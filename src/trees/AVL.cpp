// AVL.cpp - implementation of AVL from AVL.h.
#include "trees/AVL.h"
#include <cstdio>

AVL::Node::Node(int k, int v) : key(k), value(v), left(0), right(0), height(1) {}

AVL::AVL() : root(0), count(0), rotLL(0), rotRR(0), rotLR(0), rotRL(0) {}
AVL::~AVL() { destroy(root); }

int AVL::heightOf(Node* n) { return n ? n->height : 0; }

void AVL::updateHeight(Node* n) {
    int a = heightOf(n->left);
    int b = heightOf(n->right);
    n->height = (a > b ? a : b) + 1;
}

int AVL::balanceOf(Node* n) {
    return n ? heightOf(n->left) - heightOf(n->right) : 0;
}

AVL::Node* AVL::rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

AVL::Node* AVL::rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

AVL::Node* AVL::rebalance(Node* n) {
    updateHeight(n);
    int b = balanceOf(n);
    if (b > 1) {
        if (balanceOf(n->left) >= 0) {
            ++rotLL;
            return rotateRight(n);
        }
        ++rotLR;
        n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (b < -1) {
        if (balanceOf(n->right) <= 0) {
            ++rotRR;
            return rotateLeft(n);
        }
        ++rotRL;
        n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

AVL::Node* AVL::insertNode(Node* n, int key, int value) {
    if (!n) {
        ++count;
        return new Node(key, value);
    }
    if (key < n->key) n->left = insertNode(n->left, key, value);
    else if (n->key < key) n->right = insertNode(n->right, key, value);
    else {
        n->value = value;  // duplicate key: update value
        return n;
    }
    return rebalance(n);
}

AVL::Node* AVL::deleteNode(Node* n, int key, bool& removed) {
    if (!n) return 0;
    if (key < n->key) n->left = deleteNode(n->left, key, removed);
    else if (n->key < key) n->right = deleteNode(n->right, key, removed);
    else {
        removed = true;
        if (!n->left || !n->right) {
            Node* c = n->left ? n->left : n->right;
            delete n;
            return c;
        }
        // Two children: inorder successor replaces this node.
        Node* s = n->right;
        while (s->left) s = s->left;
        int succKey = s->key;
        n->key = succKey;
        n->value = s->value;
        bool dummy = false;
        n->right = deleteNode(n->right, succKey, dummy);
    }
    return rebalance(n);
}

void AVL::destroy(Node* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void AVL::inorderCollect(Node* n, int* keysOut, int* valsOut, int& filled) {
    if (!n) return;
    inorderCollect(n->left, keysOut, valsOut, filled);
    keysOut[filled] = n->key;
    if (valsOut) valsOut[filled] = n->value;
    ++filled;
    inorderCollect(n->right, keysOut, valsOut, filled);
}

void AVL::reverseCollect(Node* n, int* keysOut, int* valsOut, int& filled, int& left) {
    if (!n || left <= 0) return;
    reverseCollect(n->right, keysOut, valsOut, filled, left);
    if (left > 0) {
        keysOut[filled] = n->key;
        if (valsOut) valsOut[filled] = n->value;
        ++filled;
        --left;
    }
    reverseCollect(n->left, keysOut, valsOut, filled, left);
}

bool AVL::checkNode(Node* n, int& h) {
    if (!n) {
        h = 0;
        return true;
    }
    int hl, hr;
    if (!checkNode(n->left, hl) || !checkNode(n->right, hr)) return false;
    if (n->left && !(n->left->key < n->key)) return false;
    if (n->right && !(n->key < n->right->key)) return false;
    int d = hl - hr;
    if (d > 1 || d < -1) return false;
    h = (hl > hr ? hl : hr) + 1;
    return h == n->height;
}

void AVL::printNode(Node* n, int depth) {
    if (!n) return;
    printNode(n->right, depth + 1);
    for (int i = 0; i < depth; ++i) printf("    ");
    printf("%d (h=%d,bf=%d)\n", n->key, n->height, balanceOf(n));
    printNode(n->left, depth + 1);
}

void AVL::insert(int key, int value) { root = insertNode(root, key, value); }

bool AVL::remove(int key) {
    bool removed = false;
    root = deleteNode(root, key, removed);
    if (removed) --count;
    return removed;
}

int* AVL::search(int key) {
    Node* c = root;
    while (c) {
        if (key < c->key) c = c->left;
        else if (c->key < key) c = c->right;
        else return &c->value;
    }
    return 0;
}

int AVL::size() const { return count; }
int AVL::height() const { return heightOf(root); }
bool AVL::isEmpty() const { return count == 0; }
bool AVL::isBalanced() const {
    int h;
    return checkNode(root, h);
}
int AVL::rotationsLL() const { return rotLL; }
int AVL::rotationsRR() const { return rotRR; }
int AVL::rotationsLR() const { return rotLR; }
int AVL::rotationsRL() const { return rotRL; }

void AVL::inorder(int* keysOut, int* valsOut, int& filled) {
    filled = 0;
    inorderCollect(root, keysOut, valsOut, filled);
}

void AVL::topN(int n, int* keysOut, int* valsOut, int& filled) {
    filled = 0;
    int left = n;
    reverseCollect(root, keysOut, valsOut, filled, left);
}

void AVL::printTree() { printNode(root, 0); }
