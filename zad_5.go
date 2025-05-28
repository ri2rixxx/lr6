package main

import (
 "fmt"
 "math/rand"
 "time"
)

type Matrix [][]int

func generatePattern(n, m, start int) Matrix {
 pattern := make(Matrix, n)
 for i := range pattern {
  pattern[i] = make([]int, m)
  for j := range pattern[i] {
   pattern[i][j] = (i + j + start) % 2
  }
 }
 return pattern
}

func countDifferences(board, pattern Matrix) int {
 diff := 0
 for i := range board {
  for j := range board[i] {
   if board[i][j] != pattern[i][j] {
    diff++
   }
  }
 }
 if diff%2 != 0 {
  return -1
 }
 return diff / 2
}

func printBoard(board Matrix) {
 for _, row := range board {
  for _, val := range row {
   fmt.Printf("%d ", val)
  }
  fmt.Println()
 }
}

func main() {
 var n, m int
 fmt.Print("Введите количество строк и столбцов: ")
 fmt.Scan(&n, &m)

 rand.Seed(time.Now().UnixNano())
 board := make(Matrix, n)
 for i := range board {
  board[i] = make([]int, m)
  for j := range board[i] {
   board[i][j] = rand.Intn(2)
  }
 }

 fmt.Println("Исходная доска:")
 printBoard(board)

 pattern1 := generatePattern(n, m, 0)
 pattern2 := generatePattern(n, m, 1)

 moves1 := countDifferences(board, pattern1)
 moves2 := countDifferences(board, pattern2)

 if moves1 == -1 && moves2 == -1 {
  fmt.Println("Невозможно достичь шахматного рисунка.")
 } else {
  min := func(a, b int) int {
   if a == -1 {
    return b
   }
   if b == -1 {
    return a
   }
   if a < b {
    return a
   }
   return b
  }
  fmt.Println("Минимальное количество ходов:", min(moves1, moves2))
 }
}
