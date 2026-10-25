# 0/1 Knapsack Activity (C)

## Input format
Test cases are separated by lines containing `#Test`. Each case starts with `n W`, followed by `n` lines of `weight value`.

## Compile
GCC / MinGW:
```bash
gcc knapsack.c -o knapsack
```

## Run normally
```bash
./knapsack input.txt
```
On Windows:
```powershell
knapsack.exe input.txt
```

## Live ASCII DP visualization
```bash
./knapsack input.txt --live
```
Control update speed in milliseconds:
```bash
./knapsack input.txt --live --delay 100
```

The program prints each answer and writes results to `result.txt`. It uses standard C library functions plus `nanosleep` on non-Windows systems for the animation delay.
