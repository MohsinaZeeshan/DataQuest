// CoreBenchmarks.h - Module A (Mohsina Zeeshan)
// Module A's part of benchmark mode. The game's "benchmark" command can call
// runCoreBenchmarks() next to Module B's sorting / BST vs AVL benchmarks.
#ifndef DATAQUEST_COREBENCHMARKS_H
#define DATAQUEST_COREBENCHMARKS_H

// All four string matchers on the SAME random text and pattern.
void runStringMatchBenchmark(int textLength = 200000);

// All three hash modes with the SAME item names (inserts + finds),
// showing time, collisions, final capacity and number of rehashes.
void runHashBenchmark(int itemCount = 5000);

// Runs both of the above.
void runCoreBenchmarks();

#endif
