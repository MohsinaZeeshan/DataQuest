// Leaderboard.h - AVL-backed leaderboard + radix / merge / quick sort export.
// The leaderboard keeps its own AVL tree keyed by (score, insertion order),
// so equal scores keep their insertion order: the earlier entry ranks higher.
// Plain classes and functions - no templates.
#ifndef TREES_LEADERBOARD_H
#define TREES_LEADERBOARD_H

struct ScoreEntry {
    int score;
    char name[32];
    ScoreEntry();
    ScoreEntry(const char* n, int s);
};

// Unique AVL key: score, then insertion order (see Leaderboard.cpp).
struct ScoreKey {
    int score;
    int seq;
};

enum SortMethod { SORT_RADIX, SORT_MERGE, SORT_QUICK };

class Leaderboard {
private:
    struct Node {
        ScoreKey key;
        ScoreEntry entry;
        Node* left;
        Node* right;
        int height;
        Node(const ScoreKey& k, const ScoreEntry& e);
    };

    Node* root;
    int count;
    int nextSeq;

    static int heightOf(Node* n);
    static void updateHeight(Node* n);
    static int balanceOf(Node* n);
    static int compareKey(const ScoreKey& a, const ScoreKey& b);
    static Node* rotateRight(Node* y);
    static Node* rotateLeft(Node* x);
    Node* rebalance(Node* n);
    Node* insertNode(Node* n, const ScoreKey& k, const ScoreEntry& e);
    static void destroy(Node* n);
    static void inorderCollect(Node* n, ScoreEntry* out, int& filled);
    static void reverseCollect(Node* n, ScoreEntry* out, int& filled, int& left);
    static bool checkNode(Node* n, int& h);

    Leaderboard(const Leaderboard&);
    Leaderboard& operator=(const Leaderboard&);

public:
    Leaderboard();
    ~Leaderboard();

    void add(const char* name, int score);
    int size() const;
    bool isBalanced() const;
    int height() const;

    // Fill out[] with the top n entries (highest first).
    // Returns how many were written.
    int topN(int n, ScoreEntry* out);
    void printTop(int n);

    // Export every entry sorted by score (highest first) using the
    // chosen sort. out must hold size() entries.
    void exportSorted(ScoreEntry* out, SortMethod m);
};

#endif
