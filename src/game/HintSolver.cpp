// HintSolver.cpp - Module C (Ayesha Nehal)
#include "../../include/game/HintSolver.h"

// 1. Recursive Backtracking Solver
bool HintSolver::solveRecursiveHelper(const Dungeon& dungeon, int r, int c, int targetR, int targetC,
                                      Array2D<bool>& visited, DynArray<Coord>& path) {
    if (r == targetR && c == targetC) {
        path.pushBack(Coord(r, c));
        return true;
    }

    visited.at(r, c) = true;
    path.pushBack(Coord(r, c));

    // 4 cardinal directions: North, East, South, West
    const int dr[4] = { -1, 0, 1, 0 };
    const int dc[4] = { 0, 1, 0, -1 };

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (dungeon.inBounds(nr, nc) && dungeon.isWalkable(nr, nc) && !visited.at(nr, nc)) {
            if (solveRecursiveHelper(dungeon, nr, nc, targetR, targetC, visited, path)) {
                return true;
            }
        }
    }

    // Backtrack if no direction led to target
    path.popBack();
    return false;
}

bool HintSolver::solveRecursive(const Dungeon& dungeon, int startR, int startC, int targetR, int targetC,
                                DynArray<Coord>& outPath) {
    outPath.clear();
    if (!dungeon.inBounds(startR, startC) || !dungeon.inBounds(targetR, targetC)) return false;
    Array2D<bool> visited(dungeon.getRows(), dungeon.getCols(), false);
    return solveRecursiveHelper(dungeon, startR, startC, targetR, targetC, visited, outPath);
}

// 2. Iterative Solver using Mohsina's Stack<Coord> instead of recursion
bool HintSolver::solveIterative(const Dungeon& dungeon, int startR, int startC, int targetR, int targetC,
                               DynArray<Coord>& outPath) {
    outPath.clear();
    if (!dungeon.inBounds(startR, startC) || !dungeon.inBounds(targetR, targetC)) return false;
    if (startR == targetR && startC == targetC) {
        outPath.pushBack(Coord(startR, startC));
        return true;
    }

    int rows = dungeon.getRows();
    int cols = dungeon.getCols();
    Array2D<bool> visited(rows, cols, false);
    Array2D<Coord> parent(rows, cols, Coord(-1, -1));

    Stack<Coord> stack;
    stack.push(Coord(startR, startC));
    visited.at(startR, startC) = true;

    bool found = false;
    const int dr[4] = { -1, 0, 1, 0 };
    const int dc[4] = { 0, 1, 0, -1 };

    while (!stack.isEmpty()) {
        Coord curr = stack.pop();

        if (curr.r == targetR && curr.c == targetC) {
            found = true;
            break;
        }

        // Push unvisited walkable neighbors onto stack
        for (int i = 0; i < 4; i++) {
            int nr = curr.r + dr[i];
            int nc = curr.c + dc[i];
            if (dungeon.inBounds(nr, nc) && dungeon.isWalkable(nr, nc) && !visited.at(nr, nc)) {
                visited.at(nr, nc) = true;
                parent.at(nr, nc) = curr;
                stack.push(Coord(nr, nc));
            }
        }
    }

    if (found) {
        DynArray<Coord> revPath;
        Coord curr(targetR, targetC);
        while (curr.r != -1 && curr.c != -1) {
            revPath.pushBack(curr);
            if (curr.r == startR && curr.c == startC) break;
            curr = parent.at(curr.r, curr.c);
        }
        for (int i = revPath.size() - 1; i >= 0; i--) {
            outPath.pushBack(revPath[i]);
        }
        return true;
    }

    return false;
}

String HintSolver::getNextStepHint(const Dungeon& dungeon, int playerR, int playerC, bool useRecursive) {
    DynArray<Coord> path;
    bool ok = false;
    if (useRecursive) {
        ok = solveRecursive(dungeon, playerR, playerC, dungeon.getExitR(), dungeon.getExitC(), path);
    } else {
        ok = solveIterative(dungeon, playerR, playerC, dungeon.getExitR(), dungeon.getExitC(), path);
    }

    if (!ok || path.size() < 2) {
        return String("No clear path to the dungeon exit could be found from here!");
    }

    Coord nextStep = path[1];
    String dir;
    if (nextStep.r < playerR) dir = "NORTH";
    else if (nextStep.r > playerR) dir = "SOUTH";
    else if (nextStep.c < playerC) dir = "WEST";
    else if (nextStep.c > playerC) dir = "EAST";

    String msg = String("Dungeon Hint: Proceed ") + dir +
                 String(" (steps remaining to exit: ") +
                 String::fromInt(path.size() - 1) + String(").");
    return msg;
}

// 3. Recursion Demos
long long HintSolver::factorial(int n) {
    if (n <= 1) return 1;
    return (long long)n * factorial(n - 1);
}

long long HintSolver::fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void HintSolver::towerOfHanoi(int n, char fromRod, char toRod, char auxRod, DynArray<String>& moveLog) {
    if (n == 0) return;
    towerOfHanoi(n - 1, fromRod, auxRod, toRod, moveLog);
    String step = String("Move disk ") + String::fromInt(n) +
                  String(" from rod ") + String(fromRod) +
                  String(" to rod ") + String(toRod);
    moveLog.pushBack(step);
    towerOfHanoi(n - 1, auxRod, toRod, fromRod, moveLog);
}

void HintSolver::runDemos(std::ostream& os) {
    os << "=== Recursion Demonstration Suite ===\n";
    os << "1. Factorial: 6! = " << factorial(6) << "\n";
    os << "2. Fibonacci: fib(10) = " << fibonacci(10) << "\n";
    os << "3. Tower of Hanoi (3 disks):\n";
    DynArray<String> hanoiMoves;
    towerOfHanoi(3, 'A', 'C', 'B', hanoiMoves);
    for (int i = 0; i < hanoiMoves.size(); i++) {
        os << "   Step " << (i + 1) << ": " << hanoiMoves[i] << "\n";
    }
}
