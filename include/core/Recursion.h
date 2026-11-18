// Recursion.h - Module A (Mohsina Zeeshan)
// Recursion demos for the report: factorial, Fibonacci and Tower of Hanoi.
// Every recursive function needs a BASE CASE (stops) and a RECURSIVE CASE
// (calls itself on a smaller problem).
#ifndef DATAQUEST_RECURSION_H
#define DATAQUEST_RECURSION_H

// n! = n * (n-1)!, base case 0! = 1.          Time O(n), stack space O(n)
long long factorial(int n);

// fib(n) = fib(n-1) + fib(n-2), base cases fib(0)=0, fib(1)=1.
// Plain recursion repeats the same work:       Time O(2^n), stack space O(n)
long long fibonacci(int n);

// Same answer, but each fib(k) is stored the first time it is computed
// (memoization), so it is only calculated once. Time O(n), space O(n)
long long fibonacciMemo(int n);

// Moves n disks from `from` to `to` using `via`. Prints each move if
// printMoves is true. Returns the number of moves (always 2^n - 1).
// Time O(2^n), stack space O(n)
long long towerOfHanoi(int n, char from, char to, char via, bool printMoves);

// Runs all three demos with small inputs and prints the results.
void runRecursionDemos();

#endif
