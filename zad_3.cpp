#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

// Проверка диагонального преобладания
bool hasDiagonalDominance(const std::vector<std::vector<double>>& A) {
    int n = A.size();
    for (int i = 0; i < n; ++i) {
        double diag = std::abs(A[i][i]);
        double sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (j != i) sum += std::abs(A[i][j]);
        }
        if (diag < sum) return false;
    }
    return true;
}

// Вывод матрицы
void printMatrix(const std::string& name, const std::vector<std::vector<double>>& mat) {
    std::cout << name << ":\n";
    for (const auto& row : mat) {
        for (double val : row)
            std::cout << std::setw(10) << std::fixed << std::setprecision(4) << val << " ";
        std::cout << "\n";
    }
    std::cout << "\n";
}

// Вывод вектора
void printVector(const std::string& name, const std::vector<double>& vec) {
    std::cout << name << ":\n";
    for (double val : vec)
        std::cout << std::setw(10) << std::fixed << std::setprecision(6) << val << "\n";
    std::cout << "\n";
}

// LU-разложение и решение
void LU_Decomposition(const std::vector<std::vector<double>>& A, const std::vector<double>& b) {
    int n = A.size();

    std::vector<std::vector<double>> L(n, std::vector<double>(n, 0));
    std::vector<std::vector<double>> U(n, std::vector<double>(n, 0));

    for (int i = 0; i < n; ++i) {
        for (int k = i; k < n; ++k) {
            double sum = 0;
            for (int j = 0; j < i; ++j)
                sum += L[i][j] * U[j][k];
            U[i][k] = A[i][k] - sum;
        }

        for (int k = i; k < n; ++k) {
            if (i == k)
                L[i][i] = 1;
            else {
                double sum = 0;
                for (int j = 0; j < i; ++j)
                    sum += L[k][j] * U[j][i];
                L[k][i] = (A[k][i] - sum) / U[i][i];
            }
        }
    }

    std::vector<double> y(n);
    for (int i = 0; i < n; ++i) {
        y[i] = b[i];
        for (int j = 0; j < i; ++j)
            y[i] -= L[i][j] * y[j];
    }

    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = y[i];
        for (int j = i + 1; j < n; ++j)
            x[i] -= U[i][j] * x[j];
        x[i] /= U[i][i];
    }

    std::cout << "=== Решение через LU-разложение ===\n";
    printMatrix("A", A);
    printVector("b", b);
    printMatrix("L", L);
    printMatrix("U", U);
    printVector("y (решение L·y = b)", y);
    printVector("x (решение U·x = y)", x);
}

// Метод Якоби с проверкой диагонального преобладания
void JacobiIteration(const std::vector<std::vector<double>>& A, const std::vector<double>& b, double tol = 1e-6, int maxIter = 1000) {
    int n = A.size();
    std::vector<double> x_old(n, 0.0);
    std::vector<double> x_new(n, 0.0);
    int iter = 0;
    bool converged = false;

    std::cout << "=== Решение методом простых итераций (Якоби) ===\n";

    // Проверка на диагональное преобладание
    /*if (!hasDiagonalDominance(A)) {
        std::cerr << "Ошибка: матрица не обладает диагональным преобладанием. Метод может не сойтись.\n\n";
        return;
    }*/

    while (iter < 100) {
        for (int i = 0; i < n; ++i) {
            double sigma = 0.0;
            for (int j = 0; j < n; ++j) {
                if (j != i)
                    sigma += A[i][j] * x_old[j];
            }
            x_new[i] = (b[i] - sigma) / A[i][i];
        }

        double error = 0.0;
        for (int i = 0; i < n; ++i)
            error += std::abs(x_new[i] - x_old[i]);

        if (error < tol) {
            converged = true;
            break;
        }

        x_old = x_new;
        ++iter;
    }

    if (converged) {
        std::cout << "Сходимость достигнута за " << iter << " итераций.\n";
        printVector("x (решение)", x_new);
    } else {
        std::cout << "Решение не сошлось за " << maxIter << " итераций.\n";
        printVector("x (решение)", x_new);
    }
}

int main() {
    std::vector<std::vector<double>> A = //{
     // {10, 1, 1},
      //{2, 10, 1},
      //{2, 2, 10}
    //};
    
      {
        {-0.88, -0.04, 0.21, -18.0},
        { 0.25, -1.23, 1.0,  -0.09},
        { 0.21,  0.11, 0.8,  -0.13},
        { 0.15, -1.31, 0.06, -1.04} 
    };

    //std::vector<double> b = {12, 13, 14};
    std::vector<double> b = {-1.24, -1.04, 2.56, -0.88};

    LU_Decomposition(A, b);
    std::cout << "\n";
    JacobiIteration(A, b);

    return 0;
}
