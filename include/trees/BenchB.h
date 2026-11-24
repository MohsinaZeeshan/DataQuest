// BenchB.h - Module B benchmarks: all sorts, and BST vs AVL.
// Call runModuleBBenchmarks() from the main-menu "benchmark" option.
// Sorting sizes: 1,000 / 10,000 / 50,000 x random / sorted / reversed.
// Times in milliseconds.
// NOTE: bubble / selection / insertion at 50,000 are slow (seconds) -
// that is the point of the benchmark.
// Plain functions - no templates.
#ifndef TREES_BENCHB_H
#define TREES_BENCHB_H

void runModuleBBenchmarks();

#endif
