// Standalone benchmark runner for Module B.
// Build: g++ -std=c++11 -O2 -Iinclude/trees tests/bench_module_b.cpp src/trees/*.cpp -o bench_b
#include "BenchB.h"

int main() {
    runModuleBBenchmarks();
    return 0;
}
