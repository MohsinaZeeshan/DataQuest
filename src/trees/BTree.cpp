// BTree.cpp - implementation of BTree and SaveArchive from BTree.h.
#include "BTree.h"
#include <cstdio>
#include <cstring>

// ---------------- BTree::Node ----------------
BTree::Node::Node(int order, bool isLeaf) : n(0), leaf(isLeaf) {
    // order keys + 1 slot for temporary overflow, order+1 child pointers.
    keys = new int[order];
    vals = new int[order];
    child = new Node*[order + 1];
    for (int i = 0; i <= order; ++i) child[i] = 0;
}

BTree::Node::~Node() {
    delete[] keys;
    delete[] vals;
    delete[] child;
}

BTree::BTree(int ord) : order(ord < 3 ? 3 : ord), count(0), splits(0) {
    root = new Node(order, true);
}

BTree::~BTree() { destroy(root); }

bool BTree::insertInto(Node* x, int key, int value, int& promotedKey, int& promotedVal, Node*& newRight) {
    int i = 0;
    while (i < x->n && x->keys[i] < key) ++i;
    if (i < x->n && !(key < x->keys[i])) {
        x->vals[i] = value;  // duplicate: update
        return false;
    }

    if (x->leaf) {
        for (int j = x->n; j > i; --j) {
            x->keys[j] = x->keys[j - 1];
            x->vals[j] = x->vals[j - 1];
        }
        x->keys[i] = key;
        x->vals[i] = value;
        x->n++;
        ++count;
    } else {
        int childKey = 0, childVal = 0;
        Node* childNode = 0;
        if (insertInto(x->child[i], key, value, childKey, childVal, childNode)) {
            for (int j = x->n; j > i; --j) {
                x->keys[j] = x->keys[j - 1];
                x->vals[j] = x->vals[j - 1];
                x->child[j + 1] = x->child[j];
            }
            x->keys[i] = childKey;
            x->vals[i] = childVal;
            x->child[i + 1] = childNode;
            x->n++;
        }
    }

    if (x->n < order) return false;  // still within capacity (max order-1 keys)

    // ---- node split ----
    int mid = x->n / 2;
    Node* r = new Node(order, x->leaf);
    for (int j = mid + 1; j < x->n; ++j) {
        r->keys[j - mid - 1] = x->keys[j];
        r->vals[j - mid - 1] = x->vals[j];
    }
    if (!x->leaf) {
        for (int j = mid + 1; j <= x->n; ++j) {
            r->child[j - mid - 1] = x->child[j];
            x->child[j] = 0;
        }
    }
    r->n = x->n - mid - 1;
    promotedKey = x->keys[mid];
    promotedVal = x->vals[mid];
    x->n = mid;
    newRight = r;
    ++splits;
    return true;
}

void BTree::destroy(Node* x) {
    if (!x) return;
    if (!x->leaf) {
        for (int i = 0; i <= x->n; ++i) destroy(x->child[i]);
    }
    delete x;
}

void BTree::printNode(Node* x, int depth) {
    if (!x) return;
    for (int i = 0; i < depth; ++i) printf("    ");
    printf("[");
    for (int i = 0; i < x->n; ++i) {
        if (i) printf(" | ");
        printf("%d", x->keys[i]);
    }
    printf("]\n");
    if (!x->leaf) {
        for (int i = 0; i <= x->n; ++i) printNode(x->child[i], depth + 1);
    }
}

void BTree::insert(int key, int value) {
    int pk = 0, pv = 0;
    Node* nr = 0;
    if (insertInto(root, key, value, pk, pv, nr)) {
        // Root split: the tree grows one level taller.
        Node* nroot = new Node(order, false);
        nroot->keys[0] = pk;
        nroot->vals[0] = pv;
        nroot->child[0] = root;
        nroot->child[1] = nr;
        nroot->n = 1;
        root = nroot;
    }
}

int* BTree::search(int key) {
    Node* x = root;
    while (x) {
        int i = 0;
        while (i < x->n && x->keys[i] < key) ++i;
        if (i < x->n && !(key < x->keys[i])) return &x->vals[i];
        if (x->leaf) return 0;
        x = x->child[i];
    }
    return 0;
}

int BTree::size() const { return count; }
int BTree::getOrder() const { return order; }
int BTree::splitCount() const { return splits; }

int BTree::height() const {
    int h = 1;
    Node* x = root;
    while (!x->leaf) {
        x = x->child[0];
        ++h;
    }
    return h;
}

void BTree::printTree() { printNode(root, 0); }

// ---------------- SaveRecord / SaveArchive ----------------
SaveRecord::SaveRecord() : id(0), score(0) { file[0] = '\0'; }

SaveRecord::SaveRecord(int i, const char* f, int s) : id(i), score(s) {
    if (f) {
        strncpy(file, f, 63);
        file[63] = '\0';
    } else {
        file[0] = '\0';
    }
}

SaveArchive::SaveArchive(int order) : tree(order), records(0), recCount(0), recCap(0) {}
SaveArchive::~SaveArchive() { delete[] records; }

void SaveArchive::grow() {
    int ncap = recCap == 0 ? 8 : recCap * 2;
    SaveRecord* nd = new SaveRecord[ncap];
    for (int i = 0; i < recCount; ++i) nd[i] = records[i];
    delete[] records;
    records = nd;
    recCap = ncap;
}

void SaveArchive::addSave(const SaveRecord& r) {
    int* idx = tree.search(r.id);
    if (idx) {
        records[*idx] = r;  // id already exists: replace the record
        return;
    }
    if (recCount == recCap) grow();
    records[recCount] = r;
    tree.insert(r.id, recCount);
    ++recCount;
}

SaveRecord* SaveArchive::findSave(int id) {
    int* idx = tree.search(id);
    if (!idx) return 0;
    return &records[*idx];
}

int SaveArchive::size() const { return recCount; }
