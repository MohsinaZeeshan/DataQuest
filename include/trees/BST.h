// BST.h - unbalanced binary search tree + item catalog (Module B).
// Insert / search / delete: O(h), O(log n) average, O(n) worst (sorted input).
// Space O(n). Height is counted in nodes (empty tree = 0).
// insert / search / remove / clear are ITERATIVE, so a degenerate tree
// (sorted input, depth n) cannot overflow the call stack.
// Keys and values are plain ints here: this is the concrete, non-template
// version of the class, so it can live in a normal .h / .cpp pair.
#ifndef TREES_BST_H
#define TREES_BST_H

#include "Item.h"

class BST {
private:
    struct Node {
        int key;
        int value;
        Node* left;
        Node* right;
        Node(int k, int v);
    };

    Node* root;
    int count;

    // Recursive helpers used only for traversals / checks / printing.
    // They walk at most the height of the tree, which is fine for these.
    static void inorderCollect(Node* n, int* keysOut, int* valsOut, int& filled);
    static void preorderCollect(Node* n, int* keysOut, int& filled);
    static void postorderCollect(Node* n, int* keysOut, int& filled);
    static void levelCollect(Node* n, int level, int* keysOut, int& filled);
    static int heightOf(Node* n);
    static bool isFullNode(Node* n);
    static bool isCompleteNode(Node* n, int index, int total);
    static void printNode(Node* n, int depth);

    BST(const BST&);
    BST& operator=(const BST&);

public:
    BST();
    ~BST();

    // Destroy without recursion: rotate left children up, delete root, repeat.
    void clear();

    int size() const;
    bool isEmpty() const;

    // Insert. If the key already exists its value is replaced.
    // Returns true when a new node was added, false on update.
    bool insert(int key, int value);

    // Returns a pointer to the stored value, or 0 when the key is absent.
    // Do not keep the pointer across an insert / remove.
    int* search(int key);

    // Delete: leaf, one child, and two children (inorder successor replaces
    // the node). Returns false when the key was not found.
    bool remove(int key);

    // Traversals. Each fills keysOut (and valsOut for inorder) and sets
    // filled to how many entries were written. The caller supplies an
    // array of at least size() ints.
    void inorder(int* keysOut, int* valsOut, int& filled);
    void preorder(int* keysOut, int& filled);
    void postorder(int* keysOut, int& filled);
    void levelOrder(int* keysOut, int& filled);

    int height() const;
    bool isFull() const;      // every node has 0 or 2 children
    bool isComplete() const;  // heap-style index check

    // Sideways print (right subtree on top).
    void printTree();
};

// ItemCatalog: items stored by ID in a BST, with lookup by name.
// The BST maps id -> position in a small dynamic array of Items, so the
// tree code stays in one place and the catalog only owns the CatalogItem data.
class ItemCatalog {
private:
    CatalogItem* items;
    int itemCount;
    int itemCap;
    BST byId;  // key = item id, value = index into items[]

    void grow();

    ItemCatalog(const ItemCatalog&);
    ItemCatalog& operator=(const ItemCatalog&);

public:
    ItemCatalog();
    ~ItemCatalog();

    void add(const CatalogItem& it);
    bool removeById(int id);
    CatalogItem* findById(int id);
    CatalogItem* findByName(const char* name);  // linear scan over the catalog
    int size() const;
    void printSortedById();
    void printSortedByName();
};

#endif
