package main

import (
    "bufio"       // для считывания многострочного ввода
    "crypto/rand" // для генерации случайных байтов
    "encoding/hex"
    "fmt"
    "os"
)

// S-box — таблица подстановок AES (256 байт)
var sbox = [256]byte{
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
    0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
}

// Rcon — константы раундов
var Rcon = [11]uint32{
    0x00000000, 0x01000000, 0x02000000, 0x04000000,
    0x08000000, 0x10000000, 0x20000000, 0x40000000,
    0x80000000, 0x1B000000, 0x36000000,
}

// SubWord применяет S-box к каждому байту слова
func SubWord(word uint32) uint32 {
    return uint32(sbox[word>>24])<<24 |
        uint32(sbox[(word>>16)&0xff])<<16 |
        uint32(sbox[(word>>8)&0xff])<<8 |
        uint32(sbox[word&0xff])
}

// RotWord циклически сдвигает слово на 1 байт влево
func RotWord(word uint32) uint32 {
    return (word<<8 | word>>24)
}

// KeyExpansion — расширение ключа AES-128 (4 * (10+1) = 44 слов)
func KeyExpansion(key []byte) []uint32 {
    Nk := 4
    Nb := 4
    Nr := 10

    w := make([]uint32, Nb*(Nr+1))

    for i := 0; i < Nk; i++ {
        w[i] = uint32(key[4*i])<<24 | uint32(key[4*i+1])<<16 |
            uint32(key[4*i+2])<<8 | uint32(key[4*i+3])
    }

    for i := Nk; i < Nb*(Nr+1); i++ {
        temp := w[i-1]
        if i%Nk == 0 {
            temp = SubWord(RotWord(temp)) ^ Rcon[i/Nk]
        }
        w[i] = w[i-Nk] ^ temp
    }
    return w
}

// printHex — печатает байты в hex-формате с заголовком
func printHex(label string, data []byte) {
    fmt.Printf("%s%s\n", label, hex.EncodeToString(data))
}

// encryptBlock — имитация AES_encrypt: просто копируем байты (заглушка)
func encryptBlock(input []byte, key []uint32) []byte {
    out := make([]byte, 16)
    copy(out, input)
    return out
}

// AES128CFBEncrypt — шифрование в режиме CFB
func AES128CFBEncrypt(plaintext []byte, key []byte, iv []byte) []byte {
    roundKeys := KeyExpansion(key)
    ciphertext := make([]byte, len(plaintext))
    feedback := make([]byte, 16)
    copy(feedback, iv)

    for i := 0; i < len(plaintext); i++ {
        encrypted := encryptBlock(feedback, roundKeys)
        ciphertext[i] = plaintext[i] ^ encrypted[i%16]
        feedback[i%16] = ciphertext[i]
    }
    return ciphertext
}

// AES128CFBDecrypt — расшифровка в режиме CFB
func AES128CFBDecrypt(ciphertext []byte, key []byte, iv []byte) []byte {
    roundKeys := KeyExpansion(key)
    plaintext := make([]byte, len(ciphertext))
    feedback := make([]byte, 16)
    copy(feedback, iv)

    for i := 0; i < len(ciphertext); i++ {
        encrypted := encryptBlock(feedback, roundKeys)
        plaintext[i] = ciphertext[i] ^ encrypted[i%16]
        feedback[i%16] = ciphertext[i]
    }
    return plaintext
}

// printRoundKeys — вывод всех раундовых ключей
func printRoundKeys(keys []uint32) {
    for round := 0; round <= 10; round++ {
        fmt.Printf("Round %2d: ", round)
        for i := 0; i < 4; i++ {
            word := keys[round*4+i]
            fmt.Printf("%08x ", word)
        }
        fmt.Println()
    }
}

// main — точка входа
func main() {
    // Чтение многострочного ввода
    fmt.Println("Введите текст (CTRL+D для завершения):")
    scanner := bufio.NewScanner(os.Stdin)
    var input string
    for scanner.Scan() {
        input += scanner.Text() + "\n"
    }

    // Генерация случайного ключа и IV
    key := make([]byte, 16)
    iv := make([]byte, 16)
    rand.Read(key)
    rand.Read(iv)

    // Вывод ключа и IV
    printHex("Key: ", key)
    printHex("IV:  ", iv)

    // Генерация и вывод раундовых ключей
    roundKeys := KeyExpansion(key)
    fmt.Println("\nРаундовые ключи:")
    printRoundKeys(roundKeys)

    // Шифрование
    ciphertext := AES128CFBEncrypt([]byte(input), key, iv)
    printHex("\nEncrypted: ", ciphertext)

    // Расшифровка
    decrypted := AES128CFBDecrypt(ciphertext, key, iv)
    fmt.Println("\nDecrypted text:\n" + string(decrypted))
}
