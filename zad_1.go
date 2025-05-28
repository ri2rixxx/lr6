package main

import (
 "bufio"
 "fmt"
 "math/rand"
 "os"
 "strconv"
 "strings"
 "time"
)

const (
 Size  = 20
 Steps = 30
)

// Пауза по нажатию Enter
func waitForEnter(message string) {
 fmt.Println(message)
 bufio.NewReader(os.Stdin).ReadBytes('\n')
}

// ЗАДАНИЕ 1
func task1() {
 fmt.Println("ЗАДАНИЕ 1: Квадратная матрица N×N (N чётное), значения от 100 до 200")
 fmt.Print("Введите N: ")
 var N int
 fmt.Scanln(&N)

 if N <= 0 || N%2 != 0 {
  fmt.Println("Ошибка: N должно быть положительным чётным числом.")
  return
 }

 matrix := make([][]int, N)
 rand.Seed(time.Now().UnixNano())

 fmt.Println("\nМатрица:")
 for i := 0; i < N; i++ {
  matrix[i] = make([]int, N)
  rowSum := 0
  for j := 0; j < N; j++ {
   matrix[i][j] = 100 + rand.Intn(101)
   rowSum += matrix[i][j]
   fmt.Printf("%5d", matrix[i][j])
  }
  fmt.Printf(" | Сумма строки: %d\n", rowSum)
 }

 fmt.Println("\nСуммы столбцов:")
 for j := 0; j < N; j++ {
  colSum := 0
  for i := 0; i < N; i++ {
   colSum += matrix[i][j]
  }
  fmt.Printf("Столбец %d: %d\n", j+1, colSum)
 }
}

// ЗАДАНИЕ 2
func task2() {
 fmt.Println("\nЗАДАНИЕ 2: Матрица M×N, без цифр 5 и 7 в числах")
 fmt.Print("Введите M и N: ")
 var M, N int
 fmt.Scanln(&M, &N)

 if M <= 0 || N <= 0 {
  fmt.Println("Ошибка: размеры должны быть положительными.")
  return
 }

 matrix := make([][]int, M)
 rand.Seed(time.Now().UnixNano() + 1)

 for i := 0; i < M; i++ {
  matrix[i] = make([]int, N)
  for j := 0; j < N; j++ {
   matrix[i][j] = 1000 + rand.Intn(2001)
  }
 }

 fmt.Println("\nМатрица:")
 for _, row := range matrix {
  for _, val := range row {
   fmt.Printf("%6d", val)
  }
  fmt.Println()
 }

 maxLen := 0
 var maxSeq []int
 maxRow := -1

 for i := 0; i < M; i++ {
  var currSeq []int
  for j := 0; j < N; j++ {
   s := strconv.Itoa(matrix[i][j])
   if !strings.Contains(s, "5") && !strings.Contains(s, "7") {
    currSeq = append(currSeq, matrix[i][j])
   } else {
    if len(currSeq) > maxLen {
     maxLen = len(currSeq)
     maxSeq = append([]int{}, currSeq...)
     maxRow = i
    }
    currSeq = nil
   }
  }
  if len(currSeq) > maxLen {
   maxLen = len(currSeq)
   maxSeq = append([]int{}, currSeq...)
   maxRow = i
  }
 }

 if len(maxSeq) > 0 {
  fmt.Printf("\nСамая длинная последовательность без 5 и 7 в строке %d:\n", maxRow+1)
  for _, val := range maxSeq {
   fmt.Printf("%d ", val)
  }
  fmt.Println()
 } else {
  fmt.Println("\nНет подходящей последовательности.")
 }
}

// ЗАДАНИЕ 3
func task3() {
 waitForEnter("\nЗАДАНИЕ 3: Клеточный автомат Конвея (20×20, 30 шагов)\nНажмите Enter для запуска...")

 grid := make([][]int, Size)
 rand.Seed(time.Now().UnixNano() + 2)

 for i := range grid {
  grid[i] = make([]int, Size)
  for j := range grid[i] {
   grid[i][j] = rand.Intn(2)
  }
 }

 for step := 0; step < Steps; step++ {
  fmt.Print("\033[H\033[2J") // очистка экрана (ANSI escape code)
  fmt.Printf("Шаг %d:\n", step+1)
  for _, row := range grid {
   for _, cell := range row {
    fmt.Print(cell, " ")
   }
   fmt.Println()
  }

  next := make([][]int, Size)
  for i := range next {
   next[i] = make([]int, Size)
   for j := range next[i] {
    count := 0
    for dx := -1; dx <= 1; dx++ {
     for dy := -1; dy <= 1; dy++ {
      if dx == 0 && dy == 0 {
       continue
      }
      ni, nj := i+dx, j+dy
      if ni >= 0 && ni < Size && nj >= 0 && nj < Size {
       count += grid[ni][nj]
      }
     }
    }
    if grid[i][j] == 1 {
     if count == 2 || count == 3 {
      next[i][j] = 1
     } else {
      next[i][j] = 0
     }
    } else {
     if count == 3 {
      next[i][j] = 1
     } else {
      next[i][j] = 0
     }
    }
   }
  }

  grid = next
  time.Sleep(300 * time.Millisecond)
 }

 fmt.Println("\nЭмуляция завершена.")
}

func main() {
 task1()
 waitForEnter("\nНажмите Enter, чтобы перейти ко 2 заданию...")
 task2()
 waitForEnter("\nНажмите Enter, чтобы перейти к 3 заданию...")
 task3()
}
