// BenchB.cpp - implementation of the Module B benchmarks in BenchB.h.
#include "BenchB.h"
#include "Sort.h"
#include "Heap.h"
#include "BST.h"
#include "AVL.h"
#include "Common.h"
#include <cstdio>
#include <cstdlib>
#include <chrono>

namespace {

struct SortEntry {
    const char* name;
    SortFn fn;
    const char* avg;
    const char* worst;
};

int randBelow(int bound) {
    return (int)((((long long)rand() << 15) ^ rand()) % bound);
}

double nowMs() {
    return std::chrono::duration<double, std::milli>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

bool isSortedArr(const int* a, int n) {
    for (int i = 1; i < n; ++i) {
        if (a[i] < a[i - 1]) return false;
    }
    return true;
}

double timeSort(SortFn f, const int* base, int n, int* work) {
    for (int i = 0; i < n; ++i) work[i] = base[i];
    double t0 = nowMs();
    f(work, n);
    double t1 = nowMs();
    if (!isSortedArr(work, n)) printf("  !! sort produced wrong output\n");
    return t1 - t0;
}

void benchSorts() {
    SortEntry sorts[] = {
        {"bubble", bubbleSort, "O(n^2)", "O(n^2)"},
        {"selection", selectionSort, "O(n^2)", "O(n^2)"},
        {"insertion", insertionSort, "O(n^2)", "O(n^2)"},
        {"comb", combSort, "~O(n log n)", "O(n^2)"},
        {"radix", radixSort, "O(d(n+b))", "O(d(n+b))"},
        {"merge", mergeSort, "O(n log n)", "O(n log n)"},
        {"quick", quickSort, "O(n log n)", "O(n^2)"},
        {"heap", heapSort, "O(n log n)", "O(n log n)"},
    };
    const int NS = 8;
    const int sizes[3] = {1000, 10000, 50000};
    const char* patterns[3] = {"random", "sorted", "reversed"};

    printf("\n=== SORTING BENCHMARK (ms) ===\n");
    printf("%-22s", "expected avg Big-O:");
    for (int s = 0; s < NS; ++s) printf("%12s", sorts[s].avg);
    printf("\n%-22s", "expected worst:");
    for (int s = 0; s < NS; ++s) printf("%12s", sorts[s].worst);
    printf("\n%-22s", "n / input");
    for (int s = 0; s < NS; ++s) printf("%12s", sorts[s].name);
    printf("\n");

    for (int si = 0; si < 3; ++si) {
        int n = sizes[si];
        int* base = new int[n];
        int* work = new int[n];
        for (int p = 0; p < 3; ++p) {
            for (int i = 0; i < n; ++i) {
                base[i] = (p == 0) ? randBelow(1000000) : (p == 1 ? i : n - i);
            }
            char label[48];
            snprintf(label, sizeof(label), "%d %s", n, patterns[p]);
            printf("%-22s", label);
            for (int s = 0; s < NS; ++s) {
                printf("%12.3f", timeSort(sorts[s].fn, base, n, work));
            }
            printf("\n");
            fflush(stdout);
        }
        delete[] base;
        delete[] work;
    }
}

void benchTrees() {
    const int sizes[3] = {1000, 5000, 10000};
    printf("\n=== BST vs AVL BENCHMARK (insert n keys, then search all) ===\n");
    printf("%-8s %-8s | %10s %8s %10s | %10s %8s %10s\n", "n", "input",
           "BST ins ms", "height", "search ms", "AVL ins ms", "height", "search ms");
    for (int si = 0; si < 3; ++si) {
        int n = sizes[si];
        int* keys = new int[n];
        for (int mode = 0; mode < 2; ++mode) {  // 0 = random, 1 = sorted
            for (int i = 0; i < n; ++i) keys[i] = i;
            if (mode == 0) {
                for (int i = n - 1; i > 0; --i) swapInt(keys[i], keys[randBelow(i + 1)]);
            }

            BST bst;
            AVL avl;
            double t0 = nowMs();
            for (int i = 0; i < n; ++i) bst.insert(keys[i], i);
            double t1 = nowMs();
            for (int i = 0; i < n; ++i) bst.search(keys[i]);
            double t2 = nowMs();
            for (int i = 0; i < n; ++i) avl.insert(keys[i], i);
            double t3 = nowMs();
            for (int i = 0; i < n; ++i) avl.search(keys[i]);
            double t4 = nowMs();
            printf("%-8d %-8s | %10.3f %8d %10.3f | %10.3f %8d %10.3f\n",
                   n, mode == 0 ? "random" : "sorted", t1 - t0, bst.height(),
                   t2 - t1, t3 - t2, avl.height(), t4 - t3);
        }
        delete[] keys;
    }
    printf("Expected: BST O(log n) random / O(n) sorted (height = n); "
           "AVL O(log n) always (height ~ 1.44 log2 n).\n");
}

}  // namespace

void runModuleBBenchmarks() {
    srand(12345);
    benchSorts();
    benchTrees();
}
