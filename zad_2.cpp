#include <iostream>            // Для ввода-вывода
#include <vector>              // Для динамического массива байт
#include <string>              // Для строк
#include <iomanip>             // Для вывода в hex формате
#include <cstdint>             // Для uint8_t, uint32_t
#include <random>              // Для генерации случайных чисел

// Таблица S-box для SubWord (256 элементов)
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
    0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
    0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
    0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
    0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
    0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
    0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
    0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
    0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
    0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
    0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
    0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
    0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
    0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
    0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
    0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Константы Rcon для ключевого расширения (11 значений)
static const uint8_t Rcon[11] = {
    0x00, // Rcon[0] не используется
    0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80,
    0x1B, 0x36
};

// Функция вывода данных в 16-ричном формате с заголовком
void printHex(const std::string &label, const std::vector<uint8_t> &data) {
    std::cout << label;                                   // Вывод заголовка
    for (auto byte : data)                                // Для каждого байта из данных
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte; // Вывод байта в hex (2 цифры с ведущими нулями)
    std::cout << std::dec << "\n";                        // Возврат в десятичный вывод и перевод строки
}

// Функция циклического сдвига слова (32-бита) на 1 байт влево
uint32_t RotWord(uint32_t word) {
    return (word << 8) | (word >> 24);                    // Сдвигаем на 8 бит влево, старший байт перемещаем в младший
}

// Функция замены каждого байта слова на значение из S-box
uint32_t SubWord(uint32_t word) {
    uint32_t result = 0;                                  // Результирующее слово
    for (int i = 0; i < 4; ++i) {                         // Для каждого из 4 байт слова
        uint8_t byte = (word >> (24 - 8 * i)) & 0xFF;    // Извлекаем i-й байт (слева направо)
        uint8_t sub = sbox[byte];                         // Подставляем значение из S-box
        result |= (sub << (24 - 8 * i));                  // Помещаем обратно в результат
    }
    return result;                                        // Возвращаем преобразованное слово
}

// Функция расширения ключа: из исходного 16-байтового ключа генерируем 44 слова (4 байта каждое) ГЕНЕРАЦИЯ РАУНДОВЫХ КЛЮЧЕЙ
void KeyExpansion(const std::vector<uint8_t> &key, std::vector<uint32_t> &roundKeys) {
    int Nk = 4;       // Количество слов в ключе (AES-128: 4 слова по 4 байта)
    int Nr = 10;      // Количество раундов шифрования (AES-128: 10)
    int Nb = 4;       // Количество слов в блоке (4 слова)
    
    // Копируем исходный ключ в первые Nk слов раундовых ключей
    for (int i = 0; i < Nk; ++i) {
        roundKeys[i] = (key[4 * i] << 24) | (key[4 * i + 1] << 16) | (key[4 * i + 2] << 8) | key[4 * i + 3];
    }

    // Генерируем оставшиеся слова ключа
    for (int i = Nk; i < Nb * (Nr + 1); ++i) {
        uint32_t temp = roundKeys[i - 1];                // Временное слово для преобразований

        if (i % Nk == 0) {                               // Каждые Nk слов применяем преобразования
            temp = SubWord(RotWord(temp)) ^ (Rcon[i / Nk] << 24);
        }
        roundKeys[i] = roundKeys[i - Nk] ^ temp;         // XOR с словом Nk назад
    }
}

// Функция вывода всех раундовых ключей
void printRoundKeys(const std::vector<uint32_t> &roundKeys) {
    int Nr = 10;    // Количество раундов
    int Nb = 4;     // Количество слов в блоке

    std::cout << "\nRound Keys:\n";
    for (int i = 0; i <= Nr; ++i) {                      // Для каждого раунда + начальный ключ
        std::cout << "Round " << i << ": ";
        for (int j = 0; j < Nb; ++j) {                    // Для каждого слова в раунде
            uint32_t word = roundKeys[i * Nb + j];
            for (int k = 3; k >= 0; --k)                  // Выводим каждый байт слова в hex
                std::cout << std::hex << std::setw(2) << std::setfill('0') << ((word >> (8 * k)) & 0xFF);
            std::cout << " ";
        }
        std::cout << std::dec << "\n";                    // Возврат к десятичному формату
    }
}

// Функция сложения по модулю 256 (XOR) для байтов
uint8_t gmul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    uint8_t hi_bit_set;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set) a ^= 0x1b;  // Полином для AES
        b >>= 1;
    }
    return p;
}

// Функция преобразования одного блока 16 байт (AES Encrypt одного блока)
void AES_EncryptBlock(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, const std::vector<uint32_t> &roundKeys) {
    // Размер блока и раундов
    const int Nb = 4;
    const int Nr = 10;

    // Копируем вход в state (4x4 байта)
    uint8_t state[4][4];
    for (int i = 0; i < 16; ++i) {
        state[i % 4][i / 4] = input[i];
    }

    // AddRoundKey для начального ключа
    for (int i = 0; i < Nb; ++i) {
        uint32_t k = roundKeys[i];
        state[0][i] ^= (k >> 24) & 0xFF;
        state[1][i] ^= (k >> 16) & 0xFF;
        state[2][i] ^= (k >> 8) & 0xFF;
        state[3][i] ^= k & 0xFF;
    }

    // Раунды 1..Nr-1
    for (int round = 1; round < Nr; ++round) {
        // SubBytes - заменяем байты через S-box
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                state[r][c] = sbox[state[r][c]];

        // ShiftRows - циклический сдвиг строк
        // строка 0: сдвиг 0
        // строка 1: сдвиг 1 влево
        // строка 2: сдвиг 2 влево
        // строка 3: сдвиг 3 влево
        for (int r = 1; r < 4; ++r) {
            uint8_t temp[4];
            for (int c = 0; c < 4; ++c)
                temp[c] = state[r][(c + r) % 4];
            for (int c = 0; c < 4; ++c)
                state[r][c] = temp[c];
        }

        // MixColumns - умножение столбцов на матрицу в поле Галуа
        for (int c = 0; c < 4; ++c) {
            uint8_t a[4];
            uint8_t b[4];
            for (int i = 0; i < 4; ++i) {
                a[i] = state[i][c];
                b[i] = (state[i][c] << 1) ^ ((state[i][c] & 0x80) ? 0x1b : 0x00);
            }
            state[0][c] = b[0] ^ a[1] ^ b[1] ^ a[2] ^ a[3];
            state[1][c] = a[0] ^ b[1] ^ a[2] ^ b[2] ^ a[3];
            state[2][c] = a[0] ^ a[1] ^ b[2] ^ a[3] ^ b[3];
            state[3][c] = a[0] ^ b[0] ^ a[1] ^ a[2] ^ b[3];
        }
        
        // AddRoundKey - добавляем раундовый ключ
        for (int i = 0; i < Nb; ++i) {
            uint32_t k = roundKeys[round * Nb + i];
            state[0][i] ^= (k >> 24) & 0xFF;
            state[1][i] ^= (k >> 16) & 0xFF;
            state[2][i] ^= (k >> 8) & 0xFF;
            state[3][i] ^= k & 0xFF;
        }
    }

    // Раунд Nr без MixColumns
    // SubBytes
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            state[r][c] = sbox[state[r][c]];

    // ShiftRows
    for (int r = 1; r < 4; ++r) {
        uint8_t temp[4];
        for (int c = 0; c < 4; ++c)
            temp[c] = state[r][(c + r) % 4];
        for (int c = 0; c < 4; ++c)
            state[r][c] = temp[c];
    }

    // AddRoundKey
    for (int i = 0; i < Nb; ++i) {
        uint32_t k = roundKeys[Nr * Nb + i];
        state[0][i] ^= (k >> 24) & 0xFF;
        state[1][i] ^= (k >> 16) & 0xFF;
        state[2][i] ^= (k >> 8) & 0xFF;
        state[3][i] ^= k & 0xFF;
    }

    // Копируем state в output
    for (int i = 0; i < 16; ++i)
        output[i] = state[i % 4][i / 4];
}

// Генерация случайного вектора инициализации 16 байт
std::vector<uint8_t> generateRandomIV() {
    std::random_device rd;                                  // Источник случайных чисел
    std::mt19937 gen(rd());                                 // Генератор случайных чисел
    std::uniform_int_distribution<uint8_t> dis(0, 255);    // Равномерное распределение байтов

    std::vector<uint8_t> iv(16);
    for (int i = 0; i < 16; ++i)
        iv[i] = dis(gen);                                   // Заполняем случайными байтами
    return iv;
}

// Генерация случайного 16-байтового ключа
std::vector<uint8_t> generateRandomKey() {
    return generateRandomIV();  // Аналогично, просто 16 случайных байт
}

// Функция шифрования всего текста в режиме CFB
void AES_CFB_Encrypt(const std::vector<uint8_t> &plaintext, std::vector<uint8_t> &ciphertext,
                     const std::vector<uint32_t> &roundKeys, std::vector<uint8_t> iv) {
    size_t len = plaintext.size();                          // Длина входных данных
    ciphertext.resize(len);                                  // Резервируем место под результат

    std::vector<uint8_t> feedback = iv;                     // Начальный feedback - IV (16 байт)

    for (size_t i = 0; i < len; ++i) {
        std::vector<uint8_t> encBlock(16);                  // Буфер для зашифрованного feedback блока
        AES_EncryptBlock(feedback, encBlock, roundKeys);    // Шифруем feedback

        ciphertext[i] = plaintext[i] ^ encBlock[0];         // XOR с первым байтом зашифрованного блока

        // Обновляем feedback: сдвигаем на 1 байт влево и добавляем новый байт ciphertext
        feedback.erase(feedback.begin());
        feedback.push_back(ciphertext[i]);
    }
}

// Функция дешифрования всего текста в режиме CFB
void AES_CFB_Decrypt(const std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &plaintext,
                     const std::vector<uint32_t> &roundKeys, std::vector<uint8_t> iv) {
    size_t len = ciphertext.size();                         // Длина входных данных
    plaintext.resize(len);                                   // Резервируем место под результат

    std::vector<uint8_t> feedback = iv;                     // Начальный feedback - IV

    for (size_t i = 0; i < len; ++i) {
        std::vector<uint8_t> encBlock(16);                  // Буфер для зашифрованного feedback блока
        AES_EncryptBlock(feedback, encBlock, roundKeys);    // Шифруем feedback

        plaintext[i] = ciphertext[i] ^ encBlock[0];         // XOR с первым байтом зашифрованного блока

        // Обновляем feedback: сдвигаем на 1 байт и добавляем текущий байт ciphertext
        feedback.erase(feedback.begin());
        feedback.push_back(ciphertext[i]);
    }
}

int main() {
    std::cout << "Введите многострочный текст для шифрования (конец ввода Ctrl+D):\n";
    
    // Считываем многострочный ввод
    std::string line, fulltext;
    while (std::getline(std::cin, line)) {
        fulltext += line + "\n";                              // Добавляем каждую строку с символом перевода строки
    }
    if (!fulltext.empty()) {
        fulltext.pop_back();                                  // Удаляем последний лишний \n
    }

    if (fulltext.empty()) {
        std::cerr << "Ошибка: введён пустой текст.\n";
        return 1;
    }

    // Переводим строку в вектор байтов
    std::vector<uint8_t> plaintext(fulltext.begin(), fulltext.end());

    // Генерируем случайный ключ и IV
    std::vector<uint8_t> key = generateRandomKey();
    std::vector<uint8_t> iv = generateRandomIV();

    // Выводим ключ и IV
    printHex("Сгенерированный ключ: ", key);
    printHex("Сгенерированный IV:  ", iv);

    // Расширяем ключ
    std::vector<uint32_t> roundKeys(44);                    // 4 слова * (10 раундов + 1)
    KeyExpansion(key, roundKeys);

    // Выводим все раундовые ключи
    printRoundKeys(roundKeys);

    // Шифруем текст в режиме CFB
    std::vector<uint8_t> ciphertext;
    AES_CFB_Encrypt(plaintext, ciphertext, roundKeys, iv);

    // Выводим шифротекст в hex
    printHex("\nЗашифрованный текст (hex): ", ciphertext);

    // Дешифруем обратно
    std::vector<uint8_t> decryptedtext;
    AES_CFB_Decrypt(ciphertext, decryptedtext, roundKeys, iv);

    // Выводим дешифрованный текст
    std::cout << "\nДешифрованный текст:\n";
    std::cout.write(reinterpret_cast<char*>(decryptedtext.data()), decryptedtext.size());
    std::cout << "\n";

    return 0;
}

