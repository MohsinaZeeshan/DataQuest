// BTree.h - B-tree of configurable order (Module B).
// order m = max number of children. A node holds at most m-1 keys.
// order 3 => 2-3 tree.
// Insert: a key goes into a leaf; when a node reaches m keys it SPLITS
// around the median, the median moves up to the parent, and a split of
// the root grows the tree by one level.
// Insert / search O(log n), O(m) work inside each node. Space O(n).
// Keys and values are plain ints: concrete class, normal .h / .cpp split.
#ifndef TREES_BTREE_H
#define TREES_BTREE_H

class BTree {
private:
    struct Node {
        int n;       // keys currently stored
        bool leaf;
        int* keys;
        int* vals;
        Node** child;
        Node(int order, bool isLeaf);
        ~Node();
    };

    int order;
    int count;
    int splits;
    Node* root;

    // Returns true if x split; pk / pv is the promoted median and
    // newRight is the new right sibling.
    bool insertInto(Node* x, int key, int value, int& promotedKey, int& promotedVal, Node*& newRight);
    static void destroy(Node* x);
    static void printNode(Node* x, int depth);

    BTree(const BTree&);
    BTree& operator=(const BTree&);

public:
    BTree(int ord = 3);
    ~BTree();

    void insert(int key, int value);
    int* search(int key);
    int size() const;
    int getOrder() const;
    int splitCount() const;
    int height() const;
    void printTree();
};

// SaveRecord: one saved game, keyed by save ID in the archive.
struct SaveRecord {
    int id;
    char file[64];
    int score;
    SaveRecord();
    SaveRecord(int i, const char* f, int s);
};

// SaveArchive: B-tree of save records keyed by save ID.
// The B-tree maps id -> position in a dynamic array of SaveRecords.
class SaveArchive {
private:
    BTree tree;  // key = save id, value = index into records[]
    SaveRecord* records;
    int recCount;
    int recCap;

    void grow();

    SaveArchive(const SaveArchive&);
    SaveArchive& operator=(const SaveArchive&);

public:
    SaveArchive(int order = 3);
    ~SaveArchive();

    void addSave(const SaveRecord& r);
    SaveRecord* findSave(int id);
    int size() const;
};

#endif
