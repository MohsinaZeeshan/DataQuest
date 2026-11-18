// LinkedList.h - Module A (Mohsina Zeeshan)
// Singly, doubly and circular linked lists.
// Lists own their nodes: the destructor deletes every node.
// Copying a list is disabled (= delete) so two lists never share nodes.
#ifndef DATAQUEST_LINKEDLIST_H
#define DATAQUEST_LINKEDLIST_H

#include <iostream>
#include <stdexcept>

// ============================================================
// Singly linked list
// insertHead O(1), insertTail O(1) (tail pointer), insertAt O(n)
// deleteValue / deleteAt / search O(n), reverse O(n)
// ============================================================
template <typename T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    SinglyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~SinglyLinkedList() { clear(); }
    SinglyLinkedList(const SinglyLinkedList&) = delete;
    SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

    void insertHead(const T& value) {
        Node* n = new Node(value);
        n->next = head;
        head = n;
        if (tail == nullptr) tail = n;
        count++;
    }

    void insertTail(const T& value) {
        Node* n = new Node(value);
        if (tail == nullptr) head = tail = n;
        else { tail->next = n; tail = n; }
        count++;
    }

    // pos 0 = new head, pos == size() = new tail
    void insertAt(int pos, const T& value) {
        if (pos < 0 || pos > count) throw std::out_of_range("insertAt position");
        if (pos == 0) { insertHead(value); return; }
        if (pos == count) { insertTail(value); return; }
        Node* prev = head;
        for (int i = 0; i < pos - 1; i++) prev = prev->next;
        Node* n = new Node(value);
        n->next = prev->next;
        prev->next = n;
        count++;
    }

    // Deletes the first node holding value. Returns false if not found.
    bool deleteValue(const T& value) {
        Node* prev = nullptr;
        Node* cur = head;
        while (cur != nullptr && !(cur->data == value)) {
            prev = cur;
            cur = cur->next;
        }
        if (cur == nullptr) return false;
        unlink(prev, cur);
        return true;
    }

    void deleteAt(int pos) {
        if (pos < 0 || pos >= count) throw std::out_of_range("deleteAt position");
        Node* prev = nullptr;
        Node* cur = head;
        for (int i = 0; i < pos; i++) { prev = cur; cur = cur->next; }
        unlink(prev, cur);
    }

    // Returns the position of value, or -1.
    int search(const T& value) const {
        int i = 0;
        for (Node* cur = head; cur != nullptr; cur = cur->next, i++)
            if (cur->data == value) return i;
        return -1;
    }

    // Reverses the links in place using three pointers.
    void reverse() {
        Node* prev = nullptr;
        Node* cur = head;
        tail = head;
        while (cur != nullptr) {
            Node* next = cur->next;
            cur->next = prev;
            prev = cur;
            cur = next;
        }
        head = prev;
    }

    T& get(int pos) {
        if (pos < 0 || pos >= count) throw std::out_of_range("get position");
        Node* cur = head;
        for (int i = 0; i < pos; i++) cur = cur->next;
        return cur->data;
    }

    void print(std::ostream& os = std::cout) const {
        for (Node* cur = head; cur != nullptr; cur = cur->next) {
            os << cur->data;
            if (cur->next) os << " -> ";
        }
        os << "\n";
    }

    void clear() {
        while (head != nullptr) {
            Node* next = head->next;
            delete head;
            head = next;
        }
        tail = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
    Node* getHead() const { return head; }   // for read-only traversal (e.g. linear search)

private:
    void unlink(Node* prev, Node* cur) {
        if (prev == nullptr) head = cur->next;
        else prev->next = cur->next;
        if (cur == tail) tail = prev;
        delete cur;
        count--;
    }
};

// ============================================================
// Doubly linked list (used for undo/redo move history)
// push/pop at either end O(1), traversal forward/backward O(n)
// ============================================================
template <typename T>
class DoublyLinkedList {
public:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& d) : data(d), prev(nullptr), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    void pushFront(const T& value) {
        Node* n = new Node(value);
        n->next = head;
        if (head) head->prev = n; else tail = n;
        head = n;
        count++;
    }

    void pushBack(const T& value) {
        Node* n = new Node(value);
        n->prev = tail;
        if (tail) tail->next = n; else head = n;
        tail = n;
        count++;
    }

    T popFront() {
        if (!head) throw std::out_of_range("popFront on empty list");
        Node* old = head;
        T value = old->data;
        head = head->next;
        if (head) head->prev = nullptr; else tail = nullptr;
        delete old;
        count--;
        return value;
    }

    T popBack() {
        if (!tail) throw std::out_of_range("popBack on empty list");
        Node* old = tail;
        T value = old->data;
        tail = tail->prev;
        if (tail) tail->next = nullptr; else head = nullptr;
        delete old;
        count--;
        return value;
    }

    bool remove(const T& value) {
        for (Node* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->data == value) {
                if (cur->prev) cur->prev->next = cur->next; else head = cur->next;
                if (cur->next) cur->next->prev = cur->prev; else tail = cur->prev;
                delete cur;
                count--;
                return true;
            }
        }
        return false;
    }

    T& front() { if (!head) throw std::out_of_range("front of empty list"); return head->data; }
    T& back()  { if (!tail) throw std::out_of_range("back of empty list");  return tail->data; }

    void printForward(std::ostream& os = std::cout) const {
        for (Node* cur = head; cur; cur = cur->next) os << cur->data << (cur->next ? " <-> " : "");
        os << "\n";
    }

    void printBackward(std::ostream& os = std::cout) const {
        for (Node* cur = tail; cur; cur = cur->prev) os << cur->data << (cur->prev ? " <-> " : "");
        os << "\n";
    }

    void clear() {
        while (head) {
            Node* next = head->next;
            delete head;
            head = next;
        }
        tail = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }
};

// ============================================================
// Circular linked list (used for the combat turn cycle)
// The last node points back to the first, so next() never ends.
// insert O(1), remove O(n), advance O(1)
// ============================================================
template <typename T>
class CircularList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };
    Node* tail;      // tail->next is the head; keeping tail makes insert O(1)
    Node* cursor;    // whose turn it is
    int count;

public:
    CircularList() : tail(nullptr), cursor(nullptr), count(0) {}
    ~CircularList() { clear(); }
    CircularList(const CircularList&) = delete;
    CircularList& operator=(const CircularList&) = delete;

    // Adds at the end of the cycle (just before the head).
    void insert(const T& value) {
        Node* n = new Node(value);
        if (tail == nullptr) {
            n->next = n;           // one node points to itself
            tail = cursor = n;
        } else {
            n->next = tail->next;
            tail->next = n;
            tail = n;
        }
        count++;
    }

    bool remove(const T& value) {
        if (tail == nullptr) return false;
        Node* prev = tail;
        Node* cur = tail->next;
        for (int i = 0; i < count; i++) {
            if (cur->data == value) {
                if (count == 1) {
                    tail = cursor = nullptr;
                } else {
                    prev->next = cur->next;
                    if (cur == tail) tail = prev;
                    if (cur == cursor) cursor = cur->next;
                }
                delete cur;
                count--;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    T& current() {
        if (!cursor) throw std::out_of_range("current on empty circular list");
        return cursor->data;
    }

    // Moves to the next turn and returns it (wraps around automatically).
    T& next() {
        if (!cursor) throw std::out_of_range("next on empty circular list");
        cursor = cursor->next;
        return cursor->data;
    }

    void print(std::ostream& os = std::cout) const {
        if (!tail) { os << "(empty)\n"; return; }
        Node* cur = tail->next;
        for (int i = 0; i < count; i++, cur = cur->next) os << cur->data << " -> ";
        os << "(back to " << tail->next->data << ")\n";
    }

    void clear() {
        if (!tail) return;
        Node* cur = tail->next;
        tail->next = nullptr;     // break the circle so the loop ends
        while (cur) {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        tail = cursor = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }
};

#endif
