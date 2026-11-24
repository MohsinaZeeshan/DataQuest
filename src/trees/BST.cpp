// BST.cpp - implementation of BST and ItemCatalog from BST.h.
#include "BST.h"
#include <cstdio>
#include <cstring>

// ---------------- BST::Node ----------------
BST::Node::Node(int k, int v) : key(k), value(v), left(0), right(0) {}

BST::BST() : root(0), count(0) {}
BST::~BST() { clear(); }

void BST::clear() {
    while (root) {
        if (root->left) {
            Node* t = root->left;
            root->left = t->right;
            t->right = root;
            root = t;
        } else {
            Node* nx = root->right;
            delete root;
            root = nx;
        }
    }
    count = 0;
}

int BST::size() const { return count; }
bool BST::isEmpty() const { return count == 0; }

bool BST::insert(int key, int value) {
    if (!root) {
        root = new Node(key, value);
        ++count;
        return true;
    }
    Node* cur = root;
    for (;;) {
        if (key < cur->key) {
            if (!cur->left) {
                cur->left = new Node(key, value);
                ++count;
                return true;
            }
            cur = cur->left;
        } else if (cur->key < key) {
            if (!cur->right) {
                cur->right = new Node(key, value);
                ++count;
                return true;
            }
            cur = cur->right;
        } else {
            cur->value = value;
            return false;
        }
    }
}

int* BST::search(int key) {
    Node* cur = root;
    while (cur) {
        if (key < cur->key) cur = cur->left;
        else if (cur->key < key) cur = cur->right;
        else return &cur->value;
    }
    return 0;
}

bool BST::remove(int key) {
    Node* parent = 0;
    Node* cur = root;
    while (cur && (key < cur->key || cur->key < key)) {
        parent = cur;
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    if (!cur) return false;

    if (cur->left && cur->right) {
        // Two children: copy the inorder successor up, then delete it.
        Node* sp = cur;
        Node* s = cur->right;
        while (s->left) {
            sp = s;
            s = s->left;
        }
        cur->key = s->key;
        cur->value = s->value;
        parent = sp;
        cur = s;  // successor has at most a right child
    }
    Node* child = cur->left ? cur->left : cur->right;
    if (!parent) root = child;
    else if (parent->left == cur) parent->left = child;
    else parent->right = child;
    delete cur;
    --count;
    return true;
}

// ---------------- Traversals / checks ----------------
void BST::inorderCollect(Node* n, int* keysOut, int* valsOut, int& filled) {
    if (!n) return;
    inorderCollect(n->left, keysOut, valsOut, filled);
    keysOut[filled] = n->key;
    if (valsOut) valsOut[filled] = n->value;
    ++filled;
    inorderCollect(n->right, keysOut, valsOut, filled);
}

void BST::preorderCollect(Node* n, int* keysOut, int& filled) {
    if (!n) return;
    keysOut[filled++] = n->key;
    preorderCollect(n->left, keysOut, filled);
    preorderCollect(n->right, keysOut, filled);
}

void BST::postorderCollect(Node* n, int* keysOut, int& filled) {
    if (!n) return;
    postorderCollect(n->left, keysOut, filled);
    postorderCollect(n->right, keysOut, filled);
    keysOut[filled++] = n->key;
}

void BST::levelCollect(Node* n, int level, int* keysOut, int& filled) {
    if (!n) return;
    if (level == 1) {
        keysOut[filled++] = n->key;
    } else {
        levelCollect(n->left, level - 1, keysOut, filled);
        levelCollect(n->right, level - 1, keysOut, filled);
    }
}

int BST::heightOf(Node* n) {
    if (!n) return 0;
    int a = heightOf(n->left);
    int b = heightOf(n->right);
    return (a > b ? a : b) + 1;
}

bool BST::isFullNode(Node* n) {
    if (!n) return true;
    if (!n->left && !n->right) return true;
    if (n->left && n->right) return isFullNode(n->left) && isFullNode(n->right);
    return false;
}

bool BST::isCompleteNode(Node* n, int index, int total) {
    if (!n) return true;
    if (index >= total) return false;
    return isCompleteNode(n->left, 2 * index + 1, total) &&
           isCompleteNode(n->right, 2 * index + 2, total);
}

void BST::printNode(Node* n, int depth) {
    if (!n) return;
    printNode(n->right, depth + 1);
    for (int i = 0; i < depth; ++i) printf("    ");
    printf("%d\n", n->key);
    printNode(n->left, depth + 1);
}

void BST::inorder(int* keysOut, int* valsOut, int& filled) {
    filled = 0;
    inorderCollect(root, keysOut, valsOut, filled);
}

void BST::preorder(int* keysOut, int& filled) {
    filled = 0;
    preorderCollect(root, keysOut, filled);
}

void BST::postorder(int* keysOut, int& filled) {
    filled = 0;
    postorderCollect(root, keysOut, filled);
}

void BST::levelOrder(int* keysOut, int& filled) {
    filled = 0;
    int h = heightOf(root);
    for (int level = 1; level <= h; ++level) levelCollect(root, level, keysOut, filled);
}

int BST::height() const { return heightOf(root); }
bool BST::isFull() const { return isFullNode(root); }
bool BST::isComplete() const { return isCompleteNode(root, 0, count); }
void BST::printTree() { printNode(root, 0); }

// ---------------- ItemCatalog ----------------
ItemCatalog::ItemCatalog() : items(0), itemCount(0), itemCap(0) {}
ItemCatalog::~ItemCatalog() { delete[] items; }

void ItemCatalog::grow() {
    int ncap = itemCap == 0 ? 8 : itemCap * 2;
    Item* nd = new Item[ncap];
    for (int i = 0; i < itemCount; ++i) nd[i] = items[i];
    delete[] items;
    items = nd;
    itemCap = ncap;
}

void ItemCatalog::add(const Item& it) {
    int* idx = byId.search(it.id);
    if (idx) {
        items[*idx] = it;  // id already exists: replace the item
        return;
    }
    if (itemCount == itemCap) grow();
    items[itemCount] = it;
    byId.insert(it.id, itemCount);
    ++itemCount;
}

bool ItemCatalog::removeById(int id) {
    int* idx = byId.search(id);
    if (!idx) return false;
    int pos = *idx;
    int last = itemCount - 1;
    if (pos != last) {
        // Move the last item into the freed slot and fix its BST index.
        items[pos] = items[last];
        int* movedIdx = byId.search(items[pos].id);
        if (movedIdx) *movedIdx = pos;
    }
    byId.remove(id);
    --itemCount;
    return true;
}

Item* ItemCatalog::findById(int id) {
    int* idx = byId.search(id);
    if (!idx) return 0;
    return &items[*idx];
}

Item* ItemCatalog::findByName(const char* name) {
    if (!name) return 0;
    for (int i = 0; i < itemCount; ++i) {
        if (strcmp(items[i].name, name) == 0) return &items[i];
    }
    return 0;
}

int ItemCatalog::size() const { return itemCount; }

void ItemCatalog::printSortedById() {
    printf("Catalog sorted by ID:\n");
    if (itemCount == 0) return;
    int* keys = new int[itemCount];
    int* vals = new int[itemCount];
    int filled = 0;
    byId.inorder(keys, vals, filled);
    for (int i = 0; i < filled; ++i) {
        Item& it = items[vals[i]];
        printf("  #%-3d %-14s weight=%-3d value=%d\n", it.id, it.name, it.weight, it.value);
    }
    delete[] keys;
    delete[] vals;
}

void ItemCatalog::printSortedByName() {
    printf("Catalog sorted by name:\n");
    if (itemCount == 0) return;
    // Small catalog: insertion sort by name is enough here.
    Item* copy = new Item[itemCount];
    for (int i = 0; i < itemCount; ++i) copy[i] = items[i];
    for (int i = 1; i < itemCount; ++i) {
        Item key = copy[i];
        int j = i - 1;
        while (j >= 0 && strcmp(key.name, copy[j].name) < 0) {
            copy[j + 1] = copy[j];
            --j;
        }
        copy[j + 1] = key;
    }
    for (int i = 0; i < itemCount; ++i) {
        printf("  #%-3d %-14s weight=%-3d value=%d\n",
               copy[i].id, copy[i].name, copy[i].weight, copy[i].value);
    }
    delete[] copy;
}
