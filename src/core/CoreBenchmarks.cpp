// CoreBenchmarks.cpp - Module A (Mohsina Zeeshan)
#include "core/CoreBenchmarks.h"
#include "core/Benchmark.h"
#include "core/StringMatch.h"
#include "core/HashTable.h"
#include <cstdlib>

void runStringMatchBenchmark(int textLength) {
    // Random text over a small alphabet (a-d) so matches actually happen.
    char* buf = new char[textLength + 1];
    std::srand(42);                                  // fixed seed = same data every run
    for (int i = 0; i < textLength; i++) buf[i] = (char)('a' + std::rand() % 4);
    buf[textLength] = '\0';
    String text(buf);
    delete[] buf;

    const char* patterns[2] = { "abcdab", "abcdabcdabcdabca" };   // short and long pattern
    MatchAlgorithm algos[4] = { BRUTE_FORCE, RABIN_KARP, BOYER_MOORE, KMP };
    const char* bigO[4] = { "O(n*m)", "O(n+m) avg", "O(n/m) best", "O(n+m)" };

    for (int p = 0; p < 2; p++) {
        String pattern(patterns[p]);
        ResultsTable table(String("String matching, n = ") + String::fromInt(textLength) +
                           ", m = " + String::fromInt(pattern.length()));
        for (int a = 0; a < 4; a++) {
            DynArray<int> positions;
            MatchFunction fn = getMatcher(algos[a]);
            int found = 0;
            double ms = timeFunction([&]() { found = fn(text, pattern, positions); }, 5);
            table.addRow(matcherName(algos[a]),
                         String(bigO[a]) + ", " + String::fromInt(found) + " found", ms);
        }
        table.print();
    }
}

void runHashBenchmark(int itemCount) {
    // Build the names once so every mode gets exactly the same keys.
    DynArray<String> names(itemCount);
    for (int i = 0; i < itemCount; i++) names.pushBack(String("item") + String::fromInt(i));

    ResultsTable table(String("Hash table, ") + String::fromInt(itemCount) + " inserts + " +
                       String::fromInt(itemCount) + " finds");
    HashMode modes[3] = { CHAINING, LINEAR_PROBING, DOUBLE_HASHING };
    for (int m = 0; m < 3; m++) {
        HashTable* t = nullptr;
        double ms = timeFunction([&]() {
            delete t;                                // fresh table each repeat
            t = new HashTable(modes[m]);
            for (int i = 0; i < itemCount; i++) t->addItem(Item(names[i], 1, i));
            for (int i = 0; i < itemCount; i++) t->findItem(names[i]);
        }, 3);
        table.addRow(HashTable::modeName(modes[m]),
                     String("coll ") + String::fromInt((int)t->getCollisions()) +
                     ", cap " + String::fromInt(t->getCapacity()) +
                     ", rehash " + String::fromInt(t->getRehashCount()), ms);
        delete t;
    }
    table.print();
}

void runCoreBenchmarks() {
    runStringMatchBenchmark();
    runHashBenchmark();
}
