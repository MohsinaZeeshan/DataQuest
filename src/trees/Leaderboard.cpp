// Leaderboard.cpp - implementation of Leaderboard from Leaderboard.h.
#include "Leaderboard.h"
#include "Common.h"
#include <cstdio>
#include <cstring>

ScoreEntry::ScoreEntry() : score(0) { name[0] = '\0'; }

ScoreEntry::ScoreEntry(const char* n, int s) : score(s) {
    if (n) {
        strncpy(name, n, 31);
        name[31] = '\0';
    } else {
        name[0] = '\0';
    }
}

Leaderboard::Node::Node(const ScoreKey& k, const ScoreEntry& e)
    : key(k), entry(e), left(0), right(0), height(1) {}

Leaderboard::Leaderboard() : root(0), count(0), nextSeq(0) {}
Leaderboard::~Leaderboard() { destroy(root); }

int Leaderboard::heightOf(Node* n) { return n ? n->height : 0; }

void Leaderboard::updateHeight(Node* n) {
    int a = heightOf(n->left);
    int b = heightOf(n->right);
    n->height = (a > b ? a : b) + 1;
}

int Leaderboard::balanceOf(Node* n) {
    return n ? heightOf(n->left) - heightOf(n->right) : 0;
}

// -1 when a < b, 0 when equal, +1 when a > b. Score first, then seq.
int Leaderboard::compareKey(const ScoreKey& a, const ScoreKey& b) {
    if (a.score != b.score) return a.score < b.score ? -1 : 1;
    if (a.seq != b.seq) return a.seq < b.seq ? -1 : 1;
    return 0;
}

Leaderboard::Node* Leaderboard::rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateHeight(y);
    updateHeight(x);
    return x;
}

Leaderboard::Node* Leaderboard::rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    updateHeight(x);
    updateHeight(y);
    return y;
}

Leaderboard::Node* Leaderboard::rebalance(Node* n) {
    updateHeight(n);
    int b = balanceOf(n);
    if (b > 1) {
        if (balanceOf(n->left) >= 0) return rotateRight(n);
        n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (b < -1) {
        if (balanceOf(n->right) <= 0) return rotateLeft(n);
        n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

Leaderboard::Node* Leaderboard::insertNode(Node* n, const ScoreKey& k, const ScoreEntry& e) {
    if (!n) {
        ++count;
        return new Node(k, e);
    }
    int cmp = compareKey(k, n->key);
    if (cmp < 0) n->left = insertNode(n->left, k, e);
    else if (cmp > 0) n->right = insertNode(n->right, k, e);
    else n->entry = e;  // same key: update (cannot happen with unique seq)
    return rebalance(n);
}

void Leaderboard::destroy(Node* n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void Leaderboard::inorderCollect(Node* n, ScoreEntry* out, int& filled) {
    if (!n) return;
    inorderCollect(n->left, out, filled);
    out[filled++] = n->entry;
    inorderCollect(n->right, out, filled);
}

void Leaderboard::reverseCollect(Node* n, ScoreEntry* out, int& filled, int& left) {
    if (!n || left <= 0) return;
    reverseCollect(n->right, out, filled, left);
    if (left > 0) {
        out[filled++] = n->entry;
        --left;
    }
    reverseCollect(n->left, out, filled, left);
}

bool Leaderboard::checkNode(Node* n, int& h) {
    if (!n) {
        h = 0;
        return true;
    }
    int hl, hr;
    if (!checkNode(n->left, hl) || !checkNode(n->right, hr)) return false;
    if (n->left && compareKey(n->left->key, n->key) >= 0) return false;
    if (n->right && compareKey(n->key, n->right->key) >= 0) return false;
    int d = hl - hr;
    if (d > 1 || d < -1) return false;
    h = (hl > hr ? hl : hr) + 1;
    return h == n->height;
}

void Leaderboard::add(const char* name, int score) {
    ScoreKey k;
    k.score = score;
    k.seq = -(nextSeq++);  // earlier entry ranks higher on ties
    root = insertNode(root, k, ScoreEntry(name, score));
}

int Leaderboard::size() const { return count; }
int Leaderboard::height() const { return heightOf(root); }
bool Leaderboard::isBalanced() const {
    int h;
    return checkNode(root, h);
}

int Leaderboard::topN(int n, ScoreEntry* out) {
    int filled = 0;
    int left = n;
    reverseCollect(root, out, filled, left);
    return filled;
}

void Leaderboard::printTop(int n) {
    printf("Top %d scores:\n", n);
    int filled = 0;
    int left = n;
    // Collect first, then print with ranks (keeps printing out of the walk).
    ScoreEntry* tmp = new ScoreEntry[n > 0 ? n : 1];
    reverseCollect(root, tmp, filled, left);
    for (int i = 0; i < filled; ++i) {
        printf("  %2d. %-16s %d\n", i + 1, tmp[i].name, tmp[i].score);
    }
    delete[] tmp;
}

// ---------------- Score sorts used by exportSorted ----------------
static void swapScores(ScoreEntry& a, ScoreEntry& b) {
    ScoreEntry t = a;
    a = b;
    b = t;
}

static void radixSortScores(ScoreEntry* a, int n) {
    if (n < 2) return;
    int mn = a[0].score, mx = a[0].score;
    for (int i = 1; i < n; ++i) {
        if (a[i].score < mn) mn = a[i].score;
        if (a[i].score > mx) mx = a[i].score;
    }
    long long range = (long long)mx - mn;
    ScoreEntry* out = new ScoreEntry[n];
    for (long long exp = 1; range / exp > 0; exp *= 10) {
        int cnt[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < n; ++i) cnt[((a[i].score - mn) / exp) % 10]++;
        for (int d = 1; d < 10; ++d) cnt[d] += cnt[d - 1];
        for (int i = n - 1; i >= 0; --i) {
            int d = (int)(((a[i].score - mn) / exp) % 10);
            out[--cnt[d]] = a[i];
        }
        for (int i = 0; i < n; ++i) a[i] = out[i];
    }
    delete[] out;
}

static void mergeScoresRec(ScoreEntry* a, ScoreEntry* tmp, int lo, int hi) {
    if (hi - lo < 2) return;
    int mid = lo + (hi - lo) / 2;
    mergeScoresRec(a, tmp, lo, mid);
    mergeScoresRec(a, tmp, mid, hi);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        if (a[j].score < a[i].score) tmp[k++] = a[j++];
        else tmp[k++] = a[i++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (k = lo; k < hi; ++k) a[k] = tmp[k];
}

static void mergeSortScores(ScoreEntry* a, int n) {
    if (n < 2) return;
    ScoreEntry* tmp = new ScoreEntry[n];
    mergeScoresRec(a, tmp, 0, n);
    delete[] tmp;
}

static void quickScoresRec(ScoreEntry* a, int lo, int hi) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid].score < a[lo].score) swapScores(a[mid], a[lo]);
        if (a[hi].score < a[lo].score) swapScores(a[hi], a[lo]);
        if (a[hi].score < a[mid].score) swapScores(a[hi], a[mid]);
        int pivot = a[mid].score;
        int i = lo, j = hi;
        while (i <= j) {
            while (a[i].score < pivot) ++i;
            while (pivot < a[j].score) --j;
            if (i <= j) {
                swapScores(a[i], a[j]);
                ++i;
                --j;
            }
        }
        if (j - lo < hi - i) {
            quickScoresRec(a, lo, j);
            lo = i;
        } else {
            quickScoresRec(a, i, hi);
            hi = j;
        }
    }
}

static void quickSortScores(ScoreEntry* a, int n) {
    if (n > 1) quickScoresRec(a, 0, n - 1);
}

void Leaderboard::exportSorted(ScoreEntry* out, SortMethod m) {
    int n = 0;
    inorderCollect(root, out, n);  // ascending by score
    for (int i = 0; i < n / 2; ++i) swapScores(out[i], out[n - 1 - i]);  // scramble
    if (m == SORT_RADIX) radixSortScores(out, n);
    else if (m == SORT_MERGE) mergeSortScores(out, n);
    else quickSortScores(out, n);
    for (int i = 0; i < n / 2; ++i) swapScores(out[i], out[n - 1 - i]);  // descending
}
