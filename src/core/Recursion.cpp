// Recursion.cpp - Module A (Mohsina Zeeshan)
#include "core/Recursion.h"
#include "core/Benchmark.h"
#include <iostream>

long long factorial(int n) {
    if (n <= 1) return 1;                 // base case
    return n * factorial(n - 1);          // recursive case
}

long long fibonacci(int n) {
    if (n <= 0) return 0;                 // base cases
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Helper: memo[k] holds fib(k) once known, -1 means "not computed yet".
static long long fibMemoHelper(int n, long long* memo) {
    if (n <= 1) return (n <= 0) ? 0 : 1;
    if (memo[n] != -1) return memo[n];    // already computed: reuse it
    memo[n] = fibMemoHelper(n - 1, memo) + fibMemoHelper(n - 2, memo);
    return memo[n];
}

long long fibonacciMemo(int n) {
    if (n <= 0) return 0;
    long long* memo = new long long[n + 1];
    for (int i = 0; i <= n; i++) memo[i] = -1;
    long long result = fibMemoHelper(n, memo);
    delete[] memo;
    return result;
}

long long towerOfHanoi(int n, char from, char to, char via, bool printMoves) {
    if (n == 0) return 0;                                   // base case: nothing to move
    long long moves = towerOfHanoi(n - 1, from, via, to, printMoves);  // 1. move n-1 out of the way
    if (printMoves)
        std::cout << "  Move disk " << n << " from " << from << " to " << to << "\n";  // 2. move the biggest
    moves++;
    moves += towerOfHanoi(n - 1, via, to, from, printMoves);           // 3. move n-1 on top of it
    return moves;
}

void runRecursionDemos() {
    std::cout << "\n=== Recursion demos ===\n";

    std::cout << "Factorial:\n";
    for (int n = 0; n <= 10; n += 2)
        std::cout << "  " << n << "! = " << factorial(n) << "\n";

    std::cout << "Fibonacci (first 15): ";
    for (int n = 0; n < 15; n++) std::cout << fibonacci(n) << " ";
    std::cout << "\n";

    // Show why memoization matters: same answer, very different time.
    int n = 30;
    long long a = 0, b = 0;
    double plainMs = timeFunction([&]() { a = fibonacci(n); });
    double memoMs  = timeFunction([&]() { b = fibonacciMemo(n); });
    std::cout << "  fib(30) plain recursion = " << a << "  (" << plainMs << " ms, O(2^n))\n";
    std::cout << "  fib(30) with memo       = " << b << "  (" << memoMs << " ms, O(n))\n";

    std::cout << "Tower of Hanoi, 3 disks (A -> C using B):\n";
    long long moves = towerOfHanoi(3, 'A', 'C', 'B', true);
    std::cout << "  Total moves: " << moves << " (2^3 - 1 = 7)\n";
    std::cout << "  20 disks would need " << towerOfHanoi(20, 'A', 'C', 'B', false) << " moves\n";
}
