// Stack.h - Module A (Mohsina Zeeshan)
// Linked-list stack (LIFO). All operations O(1).
#ifndef DATAQUEST_STACK_H
#define DATAQUEST_STACK_H

#include <stdexcept>

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d, Node* n) : data(d), next(n) {}
    };
    Node* topNode;
    int count;

public:
    Stack() : topNode(nullptr), count(0) {}
    ~Stack() { clear(); }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& value) {
        topNode = new Node(value, topNode);   // new node points at old top
        count++;
    }

    T pop() {
        if (isEmpty()) throw std::out_of_range("Stack underflow");
        Node* old = topNode;
        T value = old->data;
        topNode = old->next;
        delete old;
        count--;
        return value;
    }

    T& peek() {
        if (isEmpty()) throw std::out_of_range("peek on empty stack");
        return topNode->data;
    }

    bool isEmpty() const { return topNode == nullptr; }
    int size() const { return count; }

    void clear() {
        while (!isEmpty()) pop();
    }
};

#endif
