// Queue.h - Module A (Mohsina Zeeshan)
// Linked-list queue (FIFO) with front and rear pointers. All operations O(1).
#ifndef DATAQUEST_QUEUE_H
#define DATAQUEST_QUEUE_H

#include <stdexcept>

template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* frontNode;   // dequeue from here
    Node* rearNode;    // enqueue here
    int count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}
    ~Queue() { clear(); }
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void enqueue(const T& value) {
        Node* n = new Node(value);
        if (rearNode == nullptr) frontNode = rearNode = n;
        else { rearNode->next = n; rearNode = n; }
        count++;
    }

    T dequeue() {
        if (isEmpty()) throw std::out_of_range("Queue underflow");
        Node* old = frontNode;
        T value = old->data;
        frontNode = old->next;
        if (frontNode == nullptr) rearNode = nullptr;   // queue became empty
        delete old;
        count--;
        return value;
    }

    T& front() {
        if (isEmpty()) throw std::out_of_range("front of empty queue");
        return frontNode->data;
    }

    bool isEmpty() const { return frontNode == nullptr; }
    int size() const { return count; }

    void clear() {
        while (!isEmpty()) dequeue();
    }
};

#endif
