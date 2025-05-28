package main

import (
 "fmt"
 "math"
)

// Печать матрицы
func printMatrix(name string, mat [][]float64) {
 fmt.Printf("%s:\n", name)
 for _, row := range mat {
  for _, val := range row {
   fmt.Printf("%10.4f ", val)
  }
  fmt.Println()
 }
 fmt.Println()
}

// Печать вектора
func printVector(name string, vec []float64) {
 fmt.Printf("%s:\n", name)
 for _, val := range vec {
  fmt.Printf("%10.6f\n", val)
 }
 fmt.Println()
}

// Проверка на диагональное преобладание
func isDiagonallyDominant(A [][]float64) bool {
 n := len(A)
 for i := 0; i < n; i++ {
  sum := 0.0
  for j := 0; j < n; j++ {
   if i != j {
    sum += math.Abs(A[i][j])
   }
  }
  if math.Abs(A[i][i]) < sum {
   return false
  }
 }
 return true
}

// Метод простых итераций
func simpleIterationMethod(A [][]float64, b []float64, tolerance float64, maxIterations int) ([]float64, int) {
 n := len(A)
 x := make([]float64, n)
 for iter := 0; iter < maxIterations; iter++ {
  xNew := make([]float64, n)
  for i := 0; i < n; i++ {
   sum := 0.0
   for j := 0; j < n; j++ {
    if i != j {
     sum += A[i][j] * x[j]
    }
   }
   xNew[i] = (b[i] - sum) / A[i][i]
  }

  // Проверка на сходимость
  maxDiff := 0.0
  for i := 0; i < n; i++ {
   if math.Abs(xNew[i]-x[i]) > maxDiff {
    maxDiff = math.Abs(xNew[i] - x[i])
   }
  }
  x = xNew

  if maxDiff < tolerance {
   return x, iter + 1
  }
 }
 return x, maxIterations
}

// LU-разложение и решение
func LUDecomposition(A [][]float64, b []float64) {
 n := len(A)
 L := make([][]float64, n)
 U := make([][]float64, n)
 for i := 0; i < n; i++ {
  L[i] = make([]float64, n)
  U[i] = make([]float64, n)
 }

 // Построение L и U
 for i := 0; i < n; i++ {
  for k := i; k < n; k++ {
   sum := 0.0
   for j := 0; j < i; j++ {
    sum += L[i][j] * U[j][k]
   }
   U[i][k] = A[i][k] - sum
  }
  for k := i; k < n; k++ {
   if i == k {
    L[i][i] = 1.0
   } else {
    sum := 0.0
    for j := 0; j < i; j++ {
     sum += L[k][j] * U[j][i]
    }
    L[k][i] = (A[k][i] - sum) / U[i][i]
   }
  }
 }

 // Прямой ход: L * y = b
 y := make([]float64, n)
 for i := 0; i < n; i++ {
  y[i] = b[i]
  for j := 0; j < i; j++ {
   y[i] -= L[i][j] * y[j]
  }
 }

 // Обратный ход: U * x = y
 x := make([]float64, n)
 for i := n - 1; i >= 0; i-- {
  x[i] = y[i]
  for j := i + 1; j < n; j++ {
   x[i] -= U[i][j] * x[j]
  }
  x[i] /= U[i][i]
 }

 // Вывод
 fmt.Println("Исходная система Ax = b:")
 printMatrix("A", A)
 printVector("b", b)

 printMatrix("L", L)
 printMatrix("U", U)
 printVector("y (решение L·y = b)", y)
 printVector("x (решение U·x = y)", x)
}

func main() {
 A := [][]float64{
  {-0.88, -0.04, 0.21, -18.0},
  {0.25, -1.23, 1.0, -0.09},
  {0.21, 0.11, 0.8, -0.13},
  {0.15, -1.31, 0.06, -1.04},
 }
 b := []float64{-1.24, -1.04, 2.56, -0.88}

 // Проверка на диагональное преобладание
 if isDiagonallyDominant(A) {
  fmt.Println("Матрица A является диагонально преобладающей.")
 } else {
  fmt.Println("Матрица A не является диагонально преобладающей.")
 }

 tolerance := 1e-6 // допустимая погрешность
 maxIterations := 100 // максимальное число итераций
 solution, iterations := simpleIterationMethod(A, b, tolerance, maxIterations)
 printVector("Решение методом простых итераций", solution)
 fmt.Printf("Количество итераций: %d\n", iterations)

 LUDecomposition(A, b)
}

