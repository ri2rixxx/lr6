#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib> //для фун-ий rand и srand
#include <ctime>
#include <unistd.h> //для задержки

using namespace std;

//константы для 3 подзадания
const int SIZE = 20; //размер поля
const int STEPS = 30; //количество шагов симуляции

#ifdef _WIN32 //команда очистки экрана
    #define CLEAR "clear"
#endif

//1 подзадание
void task1() {
    int N;
    cout << "ЗАДАНИЕ 1: Квадратная матрица N×N (N чётное), значения от 100 до 200\nВведите N: ";
    cin >> N;

//проверка корректности ввода
    if (N <= 0 || N % 2 != 0) {
        cout << "Ошибка: N должно быть положительным чётным числом.\n";
        return;
    }

//создание матрицы из целых чисел
    vector<vector<int>> matrix(N, vector<int>(N));
    srand(time(0)); //инициализация генератора текущим временем

    cout << "\nМатрица:\n";
    for (int i = 0; i < N; ++i) {
        int rowSum = 0; //переменная для подсчета сумм в строке
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = 100 + rand() % 101; //случайное число от 100 до 200
            rowSum += matrix[i][j];
            cout << setw(5) << matrix[i][j]; //выводим число с шириной поля 5 символов
        }
        cout << " | Сумма строки: " << rowSum << '\n';
    }

    cout << "\nСуммы столбцов:\n";
    for (int j = 0; j < N; ++j) {
        int colSum = 0; //переменная для элементов столбца 
        for (int i = 0; i < N; ++i)
            colSum += matrix[i][j];
        cout << "Столбец " << j + 1 << ": " << colSum << '\n';
    }
}

//2 подзадание
void task2() {
    int M, N;
    cout << "\nЗАДАНИЕ 2: Матрица M×N, без цифр 5 и 7 в числах\nВведите M и N: ";
    cin >> M >> N;

    if (M <= 0 || N <= 0) {
        cout << "Ошибка: размеры должны быть положительными.\n";
        return;
    }

//создание матрицы
    vector<vector<int>> matrix(M, vector<int>(N));
    srand(time(0) + 1); //инициализация генератора с другим временем (а почему бы и нет)

//заполнение матрицы рандомными числами от 1000 до 3000(1000+2000)
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j)
            matrix[i][j] = 1000 + rand() % 2001;

    cout << "\nМатрица:\n";
    for (const auto& row : matrix) {
        for (int val : row)
            cout << setw(6) << val;
        cout << '\n';
    }

    int maxLen = 0, maxRow = -1;
    vector<int> maxSeq; //для хранения самой длинной последовательности

//поиск этой последовательности
    for (int i = 0; i < M; ++i) {
        vector<int> currSeq; //текущая последовательность
        for (int j = 0; j < N; ++j) {
            int val = matrix[i][j];
            bool has5 = false, has7 = false;
            
            //проверка наличие 5 и 7 в числе val
            int tmp = val; 
            while (tmp > 0) {
                int d = tmp % 10; //последняя цифра
                if (d == 5) has5 = true;
                if (d == 7) has7 = true;
                tmp /= 10; //отбросили последнюю
            }

            if (!has5 && !has7) {
                currSeq.push_back(val);
            } else {
                if (currSeq.size() > maxLen) {
                    maxLen = currSeq.size();
                    maxSeq = currSeq;
                    maxRow = i;
                }
                currSeq.clear();
            }
        }

//после конца строки проверяем последнюю последовательность
        if (currSeq.size() > maxLen) {
            maxLen = currSeq.size();
            maxSeq = currSeq;
            maxRow = i;
        }
    }

    if (!maxSeq.empty()) {
        cout << "\nСамая длинная последовательность без 5 и 7 в строке " << maxRow + 1 << ":\n";
        for (int val : maxSeq)
            cout << val << " ";
        cout << "\n";
    } else {
        cout << "\nНет подходящей последовательности.\n";
    }
}

/*//3 подзадание
void task3() {
    cout << "\nЗАДАНИЕ 3: Клеточный автомат Конвея (20×20, 30 шагов)\n";
    cout << "Нажмите Enter для запуска...\n";
    cin.ignore(); // очищаем буфер после прошлого ввода
    cin.get();    // ждём Enter перед очисткой экрана

    // Тип для хранения сетки
    using Grid = vector<vector<int>>;


 //Инициализация сетки случайным образом с заданной плотностью живых клеток.
 // @param grid - ссылка на сетку, которую нужно заполнить.
 //@param density - вероятность появления живой клетки в каждой ячейке (по умолчанию 0.2).
 
    void initializeRandom(Grid& grid, double density=0.2) {
      for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            // Генерация случайного числа и сравнение с плотностью
            grid[i][j] = ((double)rand() / RAND_MAX) < density ? 1 : 0;
        }
    }
}


 * Вывод текущего состояния сетки на экран.
 * @param grid - константная ссылка на сетку для отображения.
 
  void printGrid(const Grid& grid) {
      system("clear");
      for (const auto& row : grid) {
          for (int cell : row) {
              if (cell)
                  cout << "O"; // Живая клетка
              else
                  cout << " "; // Мертвая клетка
          }
          cout << "\n";
      }
  }


  Подсчет количества живых соседей вокруг клетки
 Использует тороидальную топологию (границы соединены)
 @param grid - текущая сетка
 @param x - координата по вертикали
  @param y - координата по горизонтали
  @return число живых соседей
 
  int countNeighbors(const Grid& grid, int x, int y) {
      int count = 0;
      for (int dx = -1; dx <= 1; ++dx) {
          for (int dy = -1; dy <= 1; ++dy) {
              if (dx == 0 && dy == 0)
                  continue; // пропускаем саму клетку
              int nx = (x + dx + HEIGHT) % HEIGHT;
              int ny = (y + dy + WIDTH) % WIDTH;
              count += grid[nx][ny];
          }
      }
      return count;
  }
  

 * Обновление состояния сетки по правилам игры "Жизнь".
 * @param grid - ссылка на текущую сетку, которая будет обновлена.
 
  void updateGrid(Grid& grid) {
      Grid newGrid = grid; // создаем копию для обновления

      for (int i = 0; i < HEIGHT; ++i) {
          for (int j = 0; j < WIDTH; ++j) {
              int neighbors = countNeighbors(grid, i, j);
              if (grid[i][j] == 1) { // Живая клетка
                  if (neighbors < 2 || neighbors > 3)
                      newGrid[i][j] = 0; // Умирает от недостатка или избытка соседей
              } else { // Мертвая клетка
                  if (neighbors == 3)
                      newGrid[i][j] = 1; // Воскресает при наличии трех соседей
              }
          }
      }
      grid = newGrid;
  }*/

//основная прога
int main() {
    task1();
    cout << "\nНажмите Enter, чтобы перейти ко 2 заданию...\n";
    cin.ignore();
    cin.get();

    task2();
    cout << "\nНажмите Enter, чтобы перейти к 3 заданию...\n";
    cin.get();

    task3();
    return 0;
}
