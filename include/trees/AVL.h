// AVL.h - self-balancing AVL tree (Module B).
// Insert / search / delete: O(log n) worst case. Space O(n).
// Balance factor bf = height(left) - height(right); rebalance when |bf| > 1.
//   LL: bf > 1  and left child bf >= 0  -> rotate right
//   RR: bf < -1 and right child bf <= 0 -> rotate left
//   LR: bf > 1  and left child bf < 0   -> rotate left on child, then right
//   RL: bf < -1 and right child bf > 0  -> rotate right on child, then left
// Keys and values are plain ints: concrete class, normal .h / .cpp split.
#ifndef TREES_AVL_H
#define TREES_AVL_H

class AVL {
private:
    struct Node {
        int key;
        int value;
        Node* left;
        Node* right;
        int height;  // height in nodes; leaf = 1
        Node(int k, int v);
    };

    Node* root;
    int count;
    int rotLL, rotRR, rotLR, rotRL;  // rotation counters (for tests / report)

    static int heightOf(Node* n);
    static void updateHeight(Node* n);
    static int balanceOf(Node* n);
    static Node* rotateRight(Node* y);
    static Node* rotateLeft(Node* x);
    Node* rebalance(Node* n);
    Node* insertNode(Node* n, int key, int value);
    Node* deleteNode(Node* n, int key, bool& removed);
    static void destroy(Node* n);
    static void inorderCollect(Node* n, int* keysOut, int* valsOut, int& filled);
    static void reverseCollect(Node* n, int* keysOut, int* valsOut, int& filled, int& left);
    static bool checkNode(Node* n, int& h);
    static void printNode(Node* n, int depth);

    AVL(const AVL&);
    AVL& operator=(const AVL&);

public:
    AVL();
    ~AVL();

    void insert(int key, int value);
    bool remove(int key);
    int* search(int key);

    int size() const;
    int height() const;
    bool isEmpty() const;
    bool isBalanced() const;

    int rotationsLL() const;
    int rotationsRR() const;
    int rotationsLR() const;
    int rotationsRL() const;

    // Inorder (sorted order). Caller supplies arrays of at least size().
    void inorder(int* keysOut, int* valsOut, int& filled);

    // Fill keysOut / valsOut with the n LARGEST keys, largest first.
    // filled is how many were written. O(log n + n).
    void topN(int n, int* keysOut, int* valsOut, int& filled);

    // Sideways print with height and balance factor per node.
    void printTree();
};

#endif
