package main

import (
 "fmt"
 "math/bits"
)

func main() {
 var N int
 fmt.Scan(&N)

 maxBananas := -1
 bestA, bestB := 0, 0

 for a := 0; a <= N/2; a++ {
  b := N - a
  bananasA := bits.OnesCount(uint(a))
  bananasB := bits.OnesCount(uint(b))
  total := bananasA + bananasB

  if total > maxBananas || (total == maxBananas && abs(a-b) > abs(bestA-bestB)) {
   maxBananas = total
   bestA = a
   bestB = b
  }
 }

 fmt.Println(bestA, bestB)
}

func abs(x int) int {
 if x < 0 {
  return -x
 }
 return x
}
