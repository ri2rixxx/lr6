#include <iostream>
#include <vector>
#include <cstdlib> //для фун-ий rand и srand
#include <ctime> //для генерации случайных чисел

using Matrix = std::vector<std::vector<int>>; //двумерный вектор целых чисел - матрица

Matrix generatePattern(int n, int m, int start) { //генерация шахматного паттерна сдвинутого на start
    Matrix pattern(n, std::vector<int>(m)); //создаем матрицу заполненную 0
    for (int i = 0; i < n; ++i) //проход по каждой строке
        for (int j = 0; j < m; ++j) //проход по каждому столбцу в строке
            pattern[i][j] = (i + j + start) % 2; //заполняем клетку 0 и 1 по фор-ле, гарантирует чередующийся шахматный узор
    return pattern; //возвращаем заполненную матрицу-шаблон
}

//фун-ия подсчета отличий между текущей доской и заданным шаблоном
int countDifferences(const Matrix& board, const Matrix& pattern) {
    int diff = 0; //счетчик отличающихся клеток
    for (int i = 0; i < board.size(); ++i) //проход по каждой строке
        for (int j = 0; j < board[0].size(); ++j) //проход по каждому столбцу
            if (board[i][j] != pattern[i][j]) //если клетка доски не совпадает с клеткой шаблона увеличиваем счетчик
                diff++;
    
    //если число отличий нечет - преобразование невозможно, возвращаем -1, иначе возвращаем половину различий,т.к. одним изменением можно исправить 2 клетки
    return (diff % 2 != 0) ? -1 : diff / 2;
}

//фун-ия вывода матрицы на экран
void printBoard(const Matrix& board) {
    for (const auto& row : board) { //для каждой строки
        for (int val : row) //для каждого значения в строке выводим через пробел
            std::cout << val << " ";
        std::cout << "\n"; //переход на новую строку после вывода строки матрицы
    }
}

//основная программа
int main() {
    int n, m; //размеры матрицы
    std::cout << "Введите количество строк и столбцов: ";
    std::cin >> n >> m;
    
    if (n <= 0 || m <= 0) {
      std::cout << "Ошибка: количество строк и столбцов должно быть положительным числом.\n";
      return 1;
    }

    srand(time(0)); //инициализация генератора текущим временем
    Matrix board(n, std::vector<int>(m)); //создаем доску
    
    //заполняем рандомными 0 и 1
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            board[i][j] = rand() % 2;

    std::cout << "Исходная доска:\n";
    printBoard(board); //выводим исходную случайную доску

//генерируем 2 варианта идеальной доски с шахматным узором: с началом 0 и с началом 1
    Matrix pattern1 = generatePattern(n, m, 0);
    Matrix pattern2 = generatePattern(n, m, 1);

//считаем сколько изменений нужно чтобы преобразовать нашу доску в идеальную
    int moves1 = countDifferences(board, pattern1);
    int moves2 = countDifferences(board, pattern2);

    if (moves1 == -1 && moves2 == -1)
        std::cout << "Невозможно достичь шахматного рисунка.\n";
    else {
        int result = (moves1 == -1) ? moves2 : (moves2 == -1 ? moves1 : std::min(moves1, moves2));
        std::cout << "Минимальное количество ходов: " << result << "\n";
    }

    return 0;
}
