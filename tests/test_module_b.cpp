// Module B test suite (concrete classes, no templates).
// Build: g++ -std=c++11 -Wall -Wextra -g -fsanitize=address,undefined -Iinclude/trees tests/test_module_b.cpp src/trees/*.cpp -o test_b
// Run: ./test_b
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Common.h"
#include "Heap.h"
#include "Sort.h"
#include "Search.h"
#include "BST.h"
#include "AVL.h"
#include "BTree.h"
#include "Leaderboard.h"

static int passed = 0, failed = 0;
#define CHECK(cond) do { if (cond) ++passed; else { ++failed; printf("  FAIL line %d: %s\n", __LINE__, #cond); } } while (0)

static unsigned long long seed = 2024;
static int rnd(int bound) {
    seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((seed >> 33) % bound);
}

static bool sortedArr(const int* a, int n) {
    for (int i = 1; i < n; ++i) if (a[i] < a[i - 1]) return false;
    return true;
}
static long long sumArr(const int* a, int n) {
    long long s = 0;
    for (int i = 0; i < n; ++i) s += a[i];
    return s;
}

static void testHeap() {
    printf("[Heap]\n");
    MaxHeap mx;
    MinHeap mn;
    for (int i = 0; i < 200; ++i) {
        int v = rnd(1000);
        mx.insert(v);
        mn.insert(v);
    }
    CHECK(mx.size() == 200);
    int prev = 1 << 30, v, ok = 1;
    while (mx.extract(v)) { if (v > prev) ok = 0; prev = v; }
    CHECK(ok); CHECK(mx.isEmpty()); CHECK(!mx.extract(v));
    prev = -1; ok = 1;
    while (mn.extract(v)) { if (v < prev) ok = 0; prev = v; }
    CHECK(ok);
    int arr[8] = {5, 3, 9, 1, 7, 2, 8, 4};
    MaxHeap h;
    h.heapify(arr, 8);
    CHECK(h.peek() == 9);
    int hs[10] = {9, 4, 7, 1, 8, 2, 6, 3, 5, 0};
    heapSort(hs, 10);
    CHECK(sortedArr(hs, 10));
}

static void runOne(const char* name, SortFn f) {
    int sizes[5] = {0, 1, 2, 7, 500};
    for (int s = 0; s < 5; ++s) {
        int n = sizes[s];
        int* a = new int[n + 1];
        for (int mode = 0; mode < 4; ++mode) {
            for (int i = 0; i < n; ++i)
                // random (with negatives) / sorted / reversed / all-equal
                a[i] = (mode == 0) ? rnd(100) - 30 : (mode == 1 ? i : (mode == 2 ? n - i : 5));
            long long before = sumArr(a, n);
            f(a, n);
            if (!(sortedArr(a, n) && sumArr(a, n) == before)) {
                ++failed;
                printf("  FAIL %s n=%d mode=%d\n", name, n, mode);
            } else ++passed;
        }
        delete[] a;
    }
}

static void testSorts() {
    printf("[Sorts]\n");
    runOne("bubble", bubbleSort);
    runOne("selection", selectionSort);
    runOne("insertion", insertionSort);
    runOne("comb", combSort);
    runOne("radix", radixSort);
    runOne("merge", mergeSort);
    runOne("quick", quickSort);
    runOne("heap", heapSort);

    Item inv[5] = {Item(1, "sword", 10, 50), Item(2, "potion", 1, 20),
                   Item(3, "shield", 15, 40), Item(4, "key", 1, 5),
                   Item(5, "map", 2, 60)};
    sortItemsByWeight(inv, 5);
    CHECK(inv[0].weight == 1 && inv[4].weight == 15);
    CHECK(inv[0].id == 2 && inv[1].id == 4);  // stable: potion before key
    sortItemsByValue(inv, 5);
    CHECK(inv[0].value == 5 && inv[4].value == 60);
    sortItemsByValueRadix(inv, 5);
    CHECK(inv[0].value == 5);
    sortItemsByWeightQuick(inv, 5);
    CHECK(inv[0].weight == 1 && inv[4].weight == 15);
}

static void testSearch() {
    printf("[Search]\n");
    int a[10] = {2, 5, 8, 12, 16, 23, 38, 56, 72, 91};
    CHECK(linearSearch(a, 10, 23) == 5);
    CHECK(linearSearch(a, 10, 4) == -1);
    CHECK(binarySearch(a, 10, 91) == 9);
    CHECK(binarySearch(a, 10, 2) == 0);
    CHECK(binarySearch(a, 10, 7) == -1);
    CHECK(binarySearchRec(a, 0, 9, 38) == 6);
    CHECK(interpolationSearch(a, 10, 56) == 7);
    CHECK(interpolationSearch(a, 10, 1) == -1);
    CHECK(interpolationSearch(a, 10, 100) == -1);
    int eq[4] = {3, 3, 3, 3};
    CHECK(interpolationSearch(eq, 4, 3) >= 0);
    CHECK(interpolationSearch(eq, 4, 4) == -1);
    int big[1000];
    for (int i = 0; i < 1000; ++i) big[i] = i * 3;
    int allFound = 1;
    for (int i = 0; i < 1000; ++i)
        if (interpolationSearch(big, 1000, i * 3) != i ||
            binarySearch(big, 1000, i * 3) != i) allFound = 0;
    CHECK(allFound);
    ListNode n3 = {30, 0}, n2 = {20, &n3}, n1 = {10, &n2};
    CHECK(linearSearchList(&n1, 20) == &n2);
    CHECK(linearSearchList(&n1, 99) == 0);
    Item inv[3] = {Item(1, "sword", 10, 50), Item(2, "potion", 1, 20),
                   Item(3, "shield", 15, 40)};
    CHECK(findItemByName(inv, 3, "potion") == 1);
    CHECK(findItemByName(inv, 3, "bow") == -1);
    CHECK(findItemByWeight(inv, 3, 15) == 2);
    CHECK(findItemByValue(inv, 3, 50) == 0);
    sortItemsByWeight(inv, 3);
    CHECK(binarySearchItemByWeight(inv, 3, 10) == 1);
}

static void testBST() {
    printf("[BST]\n");
    BST t;
    int keys[7] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; ++i) t.insert(keys[i], keys[i] * 10);
    CHECK(t.size() == 7);
    CHECK(t.height() == 3);
    CHECK(t.isFull());
    CHECK(t.isComplete());
    CHECK(t.search(60) && *t.search(60) == 600);
    CHECK(t.search(99) == 0);
    int out[16], vals[16], n = 0;
    t.inorder(out, vals, n);
    CHECK(n == 7 && sortedArr(out, 7));
    t.preorder(out, n);
    CHECK(out[0] == 50 && out[1] == 30 && out[2] == 20);
    t.postorder(out, n);
    CHECK(out[0] == 20 && out[6] == 50);
    t.levelOrder(out, n);
    CHECK(out[0] == 50 && out[1] == 30 && out[2] == 70 && out[3] == 20 && out[6] == 80);
    printf("  tree (sideways):\n");
    t.printTree();
    CHECK(t.remove(20)); CHECK(t.search(20) == 0);            // leaf
    CHECK(t.remove(30)); CHECK(t.search(40) != 0);            // one child
    CHECK(t.remove(50)); CHECK(t.search(50) == 0);            // two children (root)
    CHECK(!t.remove(1234));
    t.inorder(out, vals, n);
    CHECK(n == 4 && sortedArr(out, 4) && out[0] == 40 && out[3] == 80);
    CHECK(t.size() == 4);
    BST e;
    CHECK(e.height() == 0); CHECK(e.isFull() && e.isComplete()); CHECK(!e.remove(1));
    BST d;
    for (int i = 0; i < 20000; ++i) d.insert(i, i);  // degenerate: must not overflow stack
    CHECK(d.height() == 20000);
    CHECK(*d.search(19999) == 19999);
    BST sk;
    sk.insert(2, 2); sk.insert(1, 1);
    CHECK(!sk.isFull()); CHECK(sk.isComplete());
    BST sk2;
    sk2.insert(1, 1); sk2.insert(2, 2);
    CHECK(!sk2.isComplete());  // right-only child is not complete
    BST big;
    for (int i = 0; i < 2000; ++i) big.insert(rnd(5000), i);
    for (int i = 0; i < 5000; ++i) big.remove(i);
    CHECK(big.size() == 0);

    ItemCatalog cat;
    cat.add(Item(3, "shield", 15, 40));
    cat.add(Item(1, "sword", 10, 50));
    cat.add(Item(2, "potion", 1, 20));
    cat.printSortedByName();
    cat.printSortedById();
    CHECK(cat.findByName("sword") && cat.findByName("sword")->id == 1);
    CHECK(cat.findById(2) && strcmp(cat.findById(2)->name, "potion") == 0);
    CHECK(cat.removeById(1));
    CHECK(cat.findByName("sword") == 0);
    CHECK(cat.size() == 2);
}

static void testAVL() {
    printf("[AVL]\n");
    { AVL t; t.insert(10, 0); t.insert(20, 0); t.insert(30, 0);
      CHECK(t.rotationsRR() == 1 && t.height() == 2); }
    { AVL t; t.insert(30, 0); t.insert(20, 0); t.insert(10, 0);
      CHECK(t.rotationsLL() == 1); }
    { AVL t; t.insert(30, 0); t.insert(10, 0); t.insert(20, 0);
      CHECK(t.rotationsLR() == 1); }
    { AVL t; t.insert(10, 0); t.insert(30, 0); t.insert(20, 0);
      CHECK(t.rotationsRL() == 1); }

    printf("  inserting 10 20 30 40 50 25 - tree after each insert:\n");
    AVL t;
    int seq[6] = {10, 20, 30, 40, 50, 25};
    for (int i = 0; i < 6; ++i) {
        t.insert(seq[i], i);
        printf("  -- after insert %d (balanced=%s)\n", seq[i], t.isBalanced() ? "yes" : "NO");
        t.printTree();
        CHECK(t.isBalanced());
    }
    AVL big;
    for (int i = 0; i < 5000; ++i) big.insert(i, i);  // sorted input
    CHECK(big.isBalanced());
    CHECK(big.height() <= 14);
    CHECK(big.size() == 5000);
    int ok = 1;
    for (int i = 0; i < 5000; ++i) if (!big.search(i)) ok = 0;
    CHECK(ok);
    for (int i = 0; i < 5000; i += 2) big.remove(i);
    CHECK(big.size() == 2500);
    CHECK(big.isBalanced());
    CHECK(big.search(2) == 0 && big.search(3) != 0);
    AVL r;
    for (int i = 0; i < 3000; ++i) r.insert(rnd(100000), i);
    CHECK(r.isBalanced());
    for (int i = 0; i < 100000; ++i) r.remove(i);
    CHECK(r.size() == 0 && r.isBalanced());
    AVL emp;
    CHECK(!emp.remove(5));
    CHECK(emp.search(5) == 0);

    Leaderboard lb;
    lb.add("Ayesha", 900);
    lb.add("Mohsina", 750);
    lb.add("Hamdan", 1200);
    lb.add("Zoraze", 750);
    lb.add("Taha", 300);
    lb.printTop(3);
    ScoreEntry top[5];
    int n = lb.topN(3, top);
    CHECK(n == 3 && top[0].score == 1200 && top[1].score == 900 && top[2].score == 750);
    CHECK(strcmp(top[2].name, "Mohsina") == 0);  // tie -> earlier entry first
    CHECK(lb.topN(10, top) == 5);
    ScoreEntry all[5];
    for (int m = 0; m < 3; ++m) {
        lb.exportSorted(all, (SortMethod)m);
        CHECK(all[0].score == 1200 && all[4].score == 300 && all[1].score == 900);
    }
}

static void testBTree() {
    printf("[B-Tree]\n");
    BTree t(3);  // order 3 = 2-3 tree
    for (int i = 1; i <= 10; ++i) t.insert(i, i * 100);
    printf("  order 3 (2-3 tree) after inserting 1..10, splits=%d, height=%d:\n",
           t.splitCount(), t.height());
    t.printTree();
    CHECK(t.size() == 10);
    CHECK(t.splitCount() > 0);
    for (int ord = 3; ord <= 7; ++ord) {
        BTree b(ord);
        int perm[400];
        for (int i = 0; i < 400; ++i) perm[i] = i;
        for (int i = 399; i > 0; --i) swapInt(perm[i], perm[rnd(i + 1)]);
        for (int i = 0; i < 400; ++i) b.insert(perm[i], perm[i] + 1);
        int ok = 1;
        for (int i = 0; i < 400; ++i) {
            int* p = b.search(i);
            if (!p || *p != i + 1) ok = 0;
        }
        CHECK(ok);
        CHECK(b.search(400) == 0);
        CHECK(b.size() == 400);
        b.insert(5, -1);
        CHECK(*b.search(5) == -1 && b.size() == 400);  // duplicate updates value
    }
    SaveArchive arc(3);
    arc.addSave(SaveRecord(7, "slot7.txt", 500));
    arc.addSave(SaveRecord(2, "slot2.txt", 320));
    arc.addSave(SaveRecord(9, "slot9.txt", 810));
    CHECK(arc.findSave(2) && arc.findSave(2)->score == 320);
    CHECK(arc.findSave(5) == 0);
    CHECK(arc.size() == 3);
}

int main() {
    testHeap();
    testSorts();
    testSearch();
    testBST();
    testAVL();
    testBTree();
    printf("\nPassed: %d   Failed: %d\n", passed, failed);
    return failed ? 1 : 0;
}
