// Benchmark.h - Module A (Mohsina Zeeshan)
// Timing harness used by every module's benchmark.
//
//   double ms = timeFunction([&]() { mergeSort(arr, n); }, 5);   // average of 5 runs
//   ResultsTable table("Sorting, n = 10000");
//   table.addRow("Merge sort", "O(n log n)", ms);
//   table.print();
#ifndef DATAQUEST_BENCHMARK_H
#define DATAQUEST_BENCHMARK_H

#include "String.h"
#include "DynArray.h"
#include <chrono>
#include <iostream>
#include <iomanip>

class Timer {
private:
    std::chrono::steady_clock::time_point startTime;
public:
    Timer() { start(); }
    void start() { startTime = std::chrono::steady_clock::now(); }
    double elapsedMs() const {
        std::chrono::duration<double, std::milli> d = std::chrono::steady_clock::now() - startTime;
        return d.count();
    }
};

// Runs fn `repeats` times and returns the average time in milliseconds.
// fn can be a normal function or a lambda.
template <typename Func>
double timeFunction(Func fn, int repeats = 1) {
    if (repeats < 1) repeats = 1;
    Timer t;
    for (int i = 0; i < repeats; i++) fn();
    return t.elapsedMs() / repeats;
}

class ResultsTable {
private:
    struct Row {
        String name;
        String detail;    // e.g. expected Big-O, or "collisions: 42"
        double ms;
    };
    String title;
    DynArray<Row> rows;

public:
    explicit ResultsTable(const String& t) : title(t) {}

    void addRow(const String& name, const String& detail, double ms) {
        Row r;
        r.name = name;
        r.detail = detail;
        r.ms = ms;
        rows.pushBack(r);
    }

    void print() const {
        std::cout << "\n=== " << title << " ===\n";
        std::cout << std::left << std::setw(24) << "Algorithm" << std::setw(26) << "Detail"
                  << std::right << std::setw(12) << "Time (ms)" << "\n";
        std::cout << "--------------------------------------------------------------\n";
        for (int i = 0; i < rows.size(); i++) {
            std::cout << std::left << std::setw(24) << rows[i].name.c_str()
                      << std::setw(26) << rows[i].detail.c_str()
                      << std::right << std::setw(12) << std::fixed << std::setprecision(3)
                      << rows[i].ms << "\n";
        }
        std::cout.unsetf(std::ios::fixed);
        std::cout << std::setprecision(6);
    }

    void clear() { rows.clear(); }
};

#endif
