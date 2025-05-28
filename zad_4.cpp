#include <iostream>
#include <vector>
#include <bitset>
#include <limits>

//Функция подсчета количества единиц в двоичном представлении числа 
int countBananas(int x) {
    int count = 0; //счетчик 1
    while (x) {
        count += x % 2; //увеличиваем счетчик, если последняя цифра 1
        x /= 2; //сдвигаем число вправо
    }
    return count; //возвращаем число 1
}

int main() {
    int N;
    std::cin >> N; //вводим N (сумма A и B)

    int maxBananas = -1; //максимальное число бананов (1 в двоичном коде)
    int bestA = 0, bestB = 0; //лучшие значения

//перебираем все возможные разбиения числа N
    for (int a = 0; a <= N / 2; ++a) {
        int b = N - a; //b дополняет a до N
        int bananasA = countBananas(a); //количество 1 в А
        int bananasB = countBananas(b); //количество единиц в В
        int total = bananasA + bananasB; //общая сумма

//если найдено большее число 1 ИЛИ при равенстве сумм 1 разница между a и b больше, то обновляем искомые значения
        if (total > maxBananas || (total == maxBananas && abs(a - b) > abs(bestA - bestB))) {
            maxBananas = total;
            bestA = a;
            bestB = b;
        }
    }

    std::cout << bestA << " " << bestB << std::endl;
    return 0;
}
