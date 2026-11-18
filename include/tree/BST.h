// BST.h - Module B stub (Muhammad Hamdan) for Module C integration
// Binary search tree item catalog: stores items by name or ID and prints in sorted order.
#ifndef DATAQUEST_BST_H
#define DATAQUEST_BST_H

#include "../core/String.h"
#include <iostream>

struct CatalogItem {
    int id;
    String name;
    int weight;
    int value;

    CatalogItem() : id(0), weight(0), value(0) {}
    CatalogItem(int i, const String& n, int w, int v) : id(i), name(n), weight(w), value(v) {}
};

class BST {
private:
    struct Node {
        CatalogItem item;
        Node* left;
        Node* right;
        Node(const CatalogItem& it) : item(it), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int count;

    Node* insertNode(Node* node, const CatalogItem& item) {
        if (!node) return new Node(item);
        if (item.name < node->item.name) node->left = insertNode(node->left, item);
        else if (node->item.name < item.name) node->right = insertNode(node->right, item);
        return node;
    }

    Node* searchNode(Node* node, const String& name) const {
        if (!node || node->item.name == name) return node;
        if (name < node->item.name) return searchNode(node->left, name);
        return searchNode(node->right, name);
    }

    void inorderPrint(Node* node, std::ostream& os) const {
        if (!node) return;
        inorderPrint(node->left, os);
        os << "  #" << node->item.id << " " << node->item.name << " (wt: "
           << node->item.weight << ", val: " << node->item.value << ")\n";
        inorderPrint(node->right, os);
    }

    void freeTree(Node* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    BST() : root(nullptr), count(0) {}
    ~BST() { clear(); }

    void insert(const CatalogItem& item) {
        root = insertNode(root, item);
        count++;
    }

    const CatalogItem* search(const String& name) const {
        Node* res = searchNode(root, name);
        return res ? &(res->item) : nullptr;
    }

    void printSorted(std::ostream& os = std::cout) const {
        if (!root) { os << "Item catalog is empty.\n"; return; }
        os << "=== Item Catalog (Sorted by Name) ===\n";
        inorderPrint(root, os);
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
