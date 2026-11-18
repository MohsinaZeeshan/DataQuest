// AVL.h - Module B stub (Muhammad Hamdan) for Module C integration
// AVL tree leaderboard: stores player scores and prints top N.
#ifndef DATAQUEST_AVL_H
#define DATAQUEST_AVL_H

#include "../core/String.h"
#include <iostream>

struct LeaderboardEntry {
    String playerName;
    int score;

    LeaderboardEntry() : score(0) {}
    LeaderboardEntry(const String& name, int s) : playerName(name), score(s) {}
};

class AVLTree {
private:
    struct Node {
        LeaderboardEntry entry;
        int height;
        Node* left;
        Node* right;
        Node(const LeaderboardEntry& e) : entry(e), height(1), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int count;

    int getHeight(Node* n) const { return n ? n->height : 0; }
    int max(int a, int b) const { return (a > b) ? a : b; }
    int getBalance(Node* n) const { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    Node* insertNode(Node* node, const LeaderboardEntry& entry) {
        if (!node) return new Node(entry);

        // Sort descending by score; if tied, sort ascending by name
        if (entry.score > node->entry.score) {
            node->left = insertNode(node->left, entry);
        } else {
            node->right = insertNode(node->right, entry);
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        // LL
        if (balance > 1 && entry.score > node->left->entry.score)
            return rightRotate(node);
        // RR
        if (balance < -1 && entry.score <= node->right->entry.score)
            return leftRotate(node);
        // LR
        if (balance > 1 && entry.score <= node->left->entry.score) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // RL
        if (balance < -1 && entry.score > node->right->entry.score) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void collectTopN(Node* node, int& remaining, int& rank, std::ostream& os) const {
        if (!node || remaining <= 0) return;
        collectTopN(node->left, remaining, rank, os);
        if (remaining > 0) {
            os << "  " << rank++ << ". " << node->entry.playerName << " - " << node->entry.score << " pts\n";
            remaining--;
        }
        collectTopN(node->right, remaining, rank, os);
    }

    void freeTree(Node* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr), count(0) {}
    ~AVLTree() { clear(); }

    void insert(const LeaderboardEntry& entry) {
        root = insertNode(root, entry);
        count++;
    }

    void insert(const String& name, int score) {
        insert(LeaderboardEntry(name, score));
    }

    void printTopN(int n = 10, std::ostream& os = std::cout) const {
        if (!root) { os << "Leaderboard is empty.\n"; return; }
        os << "=== Hall of Fame (Top Scores) ===\n";
        int remaining = n;
        int rank = 1;
        collectTopN(root, remaining, rank, os);
    }

    void clear() {
        freeTree(root);
        root = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return root == nullptr; }
};

#endif
