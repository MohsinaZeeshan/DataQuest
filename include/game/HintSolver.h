// HintSolver.h - Module C (Ayesha Nehal)
// Hint solver and recursion algorithms:
// 1. Recursive backtracking hint solver to find a path from player to exit
// 2. Iterative version using Mohsina's Stack<Coord> instead of recursion
// 3. Recursion demonstrations (Factorial, Fibonacci, Tower of Hanoi)
#ifndef DATAQUEST_HINTSOLVER_H
#define DATAQUEST_HINTSOLVER_H

#include "../core/DynArray.h"
#include "../core/Stack.h"
#include "../core/String.h"
#include "Dungeon.h"
#include <iostream>

struct Coord {
    int r, c;
    Coord() : r(0), c(0) {}
    Coord(int r_, int c_) : r(r_), c(c_) {}
    bool operator==(const Coord& o) const { return r == o.r && c == o.c; }
    bool operator!=(const Coord& o) const { return !(*this == o); }
};

class HintSolver {
private:
    static bool solveRecursiveHelper(const Dungeon& dungeon, int r, int c, int targetR, int targetC,
                                     Array2D<bool>& visited, DynArray<Coord>& path);

public:
    // 1. Recursive backtracking solver: finds path to exit
    static bool solveRecursive(const Dungeon& dungeon, int startR, int startC, int targetR, int targetC,
                               DynArray<Coord>& outPath);

    // 2. Iterative solver: uses Mohsina's Stack<Coord> instead of recursion
    static bool solveIterative(const Dungeon& dungeon, int startR, int startC, int targetR, int targetC,
                               DynArray<Coord>& outPath);

    // Returns a direction hint string (e.g. "Head EAST towards the corridor")
    static String getNextStepHint(const Dungeon& dungeon, int playerR, int playerC, bool useRecursive = true);

    // 3. Recursion demos for project report & benchmark mode
    static long long factorial(int n);
    static long long fibonacci(int n);
    static void towerOfHanoi(int n, char fromRod, char toRod, char auxRod, DynArray<String>& moveLog);
    static void runDemos(std::ostream& os = std::cout);
};

#endif
