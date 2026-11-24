// Heap.h - binary heap / priority queue + heap sort (Module B).
//   MaxHeap: largest on top (combat turn order)
//   MinHeap: smallest on top (Dijkstra, used by Module C)
// Insert O(log n), extract O(log n), peek O(1), heapify O(n),
// heapSort O(n log n), space O(n).
// Plain int heaps: concrete classes, normal .h / .cpp split.
#ifndef TREES_HEAP_H
#define TREES_HEAP_H

class MaxHeap {
private:
    int* data;
    int sz;
    int cap;

    void grow();
    void siftUp(int i);
    void siftDown(int i);

    MaxHeap(const MaxHeap&);
    MaxHeap& operator=(const MaxHeap&);

public:
    MaxHeap(int initialCap = 16);
    ~MaxHeap();

    bool isEmpty() const;
    int size() const;
    void clear();

    void insert(int v);
    // Extract the largest value. Returns false if empty.
    bool extract(int& out);
    // Precondition: !isEmpty()
    int peek() const;

    // Build a heap from an array in O(n) (Floyd's bottom-up heapify).
    void heapify(const int* arr, int n);
};

class MinHeap {
private:
    int* data;
    int sz;
    int cap;

    void grow();
    void siftUp(int i);
    void siftDown(int i);

    MinHeap(const MinHeap&);
    MinHeap& operator=(const MinHeap&);

public:
    MinHeap(int initialCap = 16);
    ~MinHeap();

    bool isEmpty() const;
    int size() const;
    void clear();

    void insert(int v);
    // Extract the smallest value. Returns false if empty.
    bool extract(int& out);
    // Precondition: !isEmpty()
    int peek() const;

    void heapify(const int* arr, int n);
};

// Heap sort, in place, ascending. O(n log n) time, O(1) extra space.
void heapSort(int* a, int n);

#endif
