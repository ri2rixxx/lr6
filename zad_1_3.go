package main

import (
    "bufio"
    "fmt"
    "math/rand"
    "os"
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

// ЗАДАНИЕ 3: Репликатор Паразит
func task3() {
    waitForEnter("\nЗАДАНИЕ 3: Репликатор Паразит (20×20, 30 шагов)\nНажмите Enter для запуска...")

    grid := make([][]int, Size)
    rand.Seed(time.Now().UnixNano() + 2)

    // Инициализация сетки с некоторыми клетками-репликаторами
    for i := range grid {
        grid[i] = make([]int, Size)
        for j := range grid[i] {
            if rand.Float64() < 0.3 {
                grid[i][j] = 1 // случайное становление живых клеток
            }
        }
    }

    for step := 0; step < Steps; step++ {
        fmt.Print("\033[H\033[2J") // очистка экрана (ANSI escape code)
        fmt.Printf("Шаг %d:\n", step+1)
        for _, row := range grid {
            for _, cell := range row {
                if cell == 1 {
                    fmt.Print("█ ")
                } else {
                    fmt.Print("  ") // пробел вместо 0
                }
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

                // Логика репликатора
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
    task3()
}
