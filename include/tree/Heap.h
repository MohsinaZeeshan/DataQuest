// Heap.h - Module B stub (Muhammad Hamdan) for Module C integration
// Max-heap for combat turn order; Min-heap for Dijkstra pathfinding.
#ifndef DATAQUEST_HEAP_H
#define DATAQUEST_HEAP_H

#include <stdexcept>

template <typename T>
class MaxHeap {
private:
    struct Element {
        T value;
        int priority;
        Element() : priority(0) {}
        Element(const T& v, int p) : value(v), priority(p) {}
    };
    Element* heap;
    int count;
    int cap;

    void grow() {
        int newCap = (cap == 0) ? 8 : cap * 2;
        Element* fresh = new Element[newCap];
        for (int i = 0; i < count; i++) fresh[i] = heap[i];
        delete[] heap;
        heap = fresh;
        cap = newCap;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i].priority > heap[parent].priority) {
                Element tmp = heap[i];
                heap[i] = heap[parent];
                heap[parent] = tmp;
                i = parent;
            } else break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int largest = i;
            if (left < count && heap[left].priority > heap[largest].priority) largest = left;
            if (right < count && heap[right].priority > heap[largest].priority) largest = right;
            if (largest != i) {
                Element tmp = heap[i];
                heap[i] = heap[largest];
                heap[largest] = tmp;
                i = largest;
            } else break;
        }
    }

public:
    MaxHeap(int initialCap = 16) : heap(nullptr), count(0), cap(0) {
        if (initialCap > 0) {
            heap = new Element[initialCap];
            cap = initialCap;
        }
    }

    ~MaxHeap() { delete[] heap; }

    MaxHeap(const MaxHeap& other) : heap(nullptr), count(other.count), cap(other.cap) {
        if (cap > 0) {
            heap = new Element[cap];
            for (int i = 0; i < count; i++) heap[i] = other.heap[i];
        }
    }

    MaxHeap& operator=(const MaxHeap& other) {
        if (this == &other) return *this;
        delete[] heap;
        count = other.count;
        cap = other.cap;
        heap = (cap > 0) ? new Element[cap] : nullptr;
        for (int i = 0; i < count; i++) heap[i] = other.heap[i];
        return *this;
    }

    void insert(const T& val, int priority) {
        if (count == cap) grow();
        heap[count] = Element(val, priority);
        siftUp(count);
        count++;
    }

    T extractMax() {
        if (isEmpty()) throw std::out_of_range("extractMax on empty MaxHeap");
        T res = heap[0].value;
        heap[0] = heap[count - 1];
        count--;
        if (count > 0) siftDown(0);
        return res;
    }

    T& peek() {
        if (isEmpty()) throw std::out_of_range("peek on empty MaxHeap");
        return heap[0].value;
    }

    int peekPriority() const {
        if (isEmpty()) throw std::out_of_range("peekPriority on empty MaxHeap");
        return heap[0].priority;
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    void clear() { count = 0; }
};

template <typename T>
class MinHeap {
private:
    struct Element {
        T value;
        int priority;
        Element() : priority(0) {}
        Element(const T& v, int p) : value(v), priority(p) {}
    };
    Element* heap;
    int count;
    int cap;

    void grow() {
        int newCap = (cap == 0) ? 8 : cap * 2;
        Element* fresh = new Element[newCap];
        for (int i = 0; i < count; i++) fresh[i] = heap[i];
        delete[] heap;
        heap = fresh;
        cap = newCap;
    }

    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[i].priority < heap[parent].priority) {
                Element tmp = heap[i];
                heap[i] = heap[parent];
                heap[parent] = tmp;
                i = parent;
            } else break;
        }
    }

    void siftDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;
            if (left < count && heap[left].priority < heap[smallest].priority) smallest = left;
            if (right < count && heap[right].priority < heap[smallest].priority) smallest = right;
            if (smallest != i) {
                Element tmp = heap[i];
                heap[i] = heap[smallest];
                heap[smallest] = tmp;
                i = smallest;
            } else break;
        }
    }

public:
    MinHeap(int initialCap = 16) : heap(nullptr), count(0), cap(0) {
        if (initialCap > 0) {
            heap = new Element[initialCap];
            cap = initialCap;
        }
    }

    ~MinHeap() { delete[] heap; }

    MinHeap(const MinHeap& other) : heap(nullptr), count(other.count), cap(other.cap) {
        if (cap > 0) {
            heap = new Element[cap];
            for (int i = 0; i < count; i++) heap[i] = other.heap[i];
        }
    }

    MinHeap& operator=(const MinHeap& other) {
        if (this == &other) return *this;
        delete[] heap;
        count = other.count;
        cap = other.cap;
        heap = (cap > 0) ? new Element[cap] : nullptr;
        for (int i = 0; i < count; i++) heap[i] = other.heap[i];
        return *this;
    }

    void insert(const T& val, int priority) {
        if (count == cap) grow();
        heap[count] = Element(val, priority);
        siftUp(count);
        count++;
    }

    T extractMin() {
        if (isEmpty()) throw std::out_of_range("extractMin on empty MinHeap");
        T res = heap[0].value;
        heap[0] = heap[count - 1];
        count--;
        if (count > 0) siftDown(0);
        return res;
    }

    T& peek() {
        if (isEmpty()) throw std::out_of_range("peek on empty MinHeap");
        return heap[0].value;
    }

    int peekPriority() const {
        if (isEmpty()) throw std::out_of_range("peekPriority on empty MinHeap");
        return heap[0].priority;
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
    void clear() { count = 0; }
};

#endif
