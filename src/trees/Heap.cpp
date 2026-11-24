// Heap.cpp - implementation of MaxHeap, MinHeap and heapSort from Heap.h.
#include "Heap.h"
#include "Common.h"

// ---------------- MaxHeap ----------------
MaxHeap::MaxHeap(int initialCap) : sz(0), cap(initialCap < 1 ? 1 : initialCap) {
    data = new int[cap];
}
MaxHeap::~MaxHeap() { delete[] data; }

bool MaxHeap::isEmpty() const { return sz == 0; }
int MaxHeap::size() const { return sz; }
void MaxHeap::clear() { sz = 0; }

void MaxHeap::grow() {
    int ncap = cap * 2;
    int* nd = new int[ncap];
    for (int i = 0; i < sz; ++i) nd[i] = data[i];
    delete[] data;
    data = nd;
    cap = ncap;
}

void MaxHeap::siftUp(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (data[i] > data[p]) {
            swapInt(data[i], data[p]);
            i = p;
        } else break;
    }
}

void MaxHeap::siftDown(int i) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, b = i;
        if (l < sz && data[l] > data[b]) b = l;
        if (r < sz && data[r] > data[b]) b = r;
        if (b == i) break;
        swapInt(data[i], data[b]);
        i = b;
    }
}

void MaxHeap::insert(int v) {
    if (sz == cap) grow();
    data[sz] = v;
    siftUp(sz++);
}

bool MaxHeap::extract(int& out) {
    if (sz == 0) return false;
    out = data[0];
    data[0] = data[--sz];
    if (sz > 0) siftDown(0);
    return true;
}

int MaxHeap::peek() const { return data[0]; }

void MaxHeap::heapify(const int* arr, int n) {
    while (cap < n) grow();
    for (int i = 0; i < n; ++i) data[i] = arr[i];
    sz = n;
    for (int i = n / 2 - 1; i >= 0; --i) siftDown(i);
}

// ---------------- MinHeap ----------------
MinHeap::MinHeap(int initialCap) : sz(0), cap(initialCap < 1 ? 1 : initialCap) {
    data = new int[cap];
}
MinHeap::~MinHeap() { delete[] data; }

bool MinHeap::isEmpty() const { return sz == 0; }
int MinHeap::size() const { return sz; }
void MinHeap::clear() { sz = 0; }

void MinHeap::grow() {
    int ncap = cap * 2;
    int* nd = new int[ncap];
    for (int i = 0; i < sz; ++i) nd[i] = data[i];
    delete[] data;
    data = nd;
    cap = ncap;
}

void MinHeap::siftUp(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (data[i] < data[p]) {
            swapInt(data[i], data[p]);
            i = p;
        } else break;
    }
}

void MinHeap::siftDown(int i) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, b = i;
        if (l < sz && data[l] < data[b]) b = l;
        if (r < sz && data[r] < data[b]) b = r;
        if (b == i) break;
        swapInt(data[i], data[b]);
        i = b;
    }
}

void MinHeap::insert(int v) {
    if (sz == cap) grow();
    data[sz] = v;
    siftUp(sz++);
}

bool MinHeap::extract(int& out) {
    if (sz == 0) return false;
    out = data[0];
    data[0] = data[--sz];
    if (sz > 0) siftDown(0);
    return true;
}

int MinHeap::peek() const { return data[0]; }

void MinHeap::heapify(const int* arr, int n) {
    while (cap < n) grow();
    for (int i = 0; i < n; ++i) data[i] = arr[i];
    sz = n;
    for (int i = n / 2 - 1; i >= 0; --i) siftDown(i);
}

// ---------------- Heap sort ----------------
static void heapSiftDownArr(int* a, int i, int n) {
    for (;;) {
        int l = 2 * i + 1, r = l + 1, big = i;
        if (l < n && a[big] < a[l]) big = l;
        if (r < n && a[big] < a[r]) big = r;
        if (big == i) break;
        swapInt(a[i], a[big]);
        i = big;
    }
}

void heapSort(int* a, int n) {
    for (int i = n / 2 - 1; i >= 0; --i) heapSiftDownArr(a, i, n);
    for (int end = n - 1; end > 0; --end) {
        swapInt(a[0], a[end]);
        heapSiftDownArr(a, 0, end);
    }
}
