# 0/1 Knapsack Activity (C)

## Compile
GCC / MinGW:
```bash
gcc knapsack.c -o knapsack
```
## Run normally
```bash
./knapsack input.txt
```
## Live ASCII DP visualization
```bash
./knapsack input.txt --live
```

The program prints each answer and writes results to `result.txt`. It uses standard C library functions plus `nanosleep` on non-Windows systems for the animation delay.
