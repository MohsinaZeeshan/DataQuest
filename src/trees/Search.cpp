// Search.cpp - implementation of the searches in Search.h.
#include "trees/Search.h"
#include <cstring>

int linearSearch(const int* a, int n, int key) {
    for (int i = 0; i < n; ++i) {
        if (a[i] == key) return i;
    }
    return -1;
}

ListNode* linearSearchList(ListNode* head, int key) {
    for (ListNode* p = head; p; p = p->next) {
        if (p->data == key) return p;
    }
    return 0;
}

int binarySearch(const int* a, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key) return mid;
        if (a[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int binarySearchRec(const int* a, int lo, int hi, int key) {
    if (lo > hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (a[mid] == key) return mid;
    if (a[mid] < key) return binarySearchRec(a, mid + 1, hi, key);
    return binarySearchRec(a, lo, mid - 1, key);
}

int interpolationSearch(const int* a, int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi && key >= a[lo] && key <= a[hi]) {
        if (a[hi] == a[lo]) return (a[lo] == key) ? lo : -1;  // avoid divide by zero
        int pos = lo + (int)(((long long)(key - a[lo]) * (hi - lo)) /
                             ((long long)a[hi] - a[lo]));
        if (a[pos] == key) return pos;
        if (a[pos] < key) lo = pos + 1;
        else hi = pos - 1;
    }
    return -1;
}

int findItemByName(const CatalogItem* a, int n, const char* name) {
    if (!name) return -1;
    for (int i = 0; i < n; ++i) {
        if (strcmp(a[i].name, name) == 0) return i;
    }
    return -1;
}

int findItemByWeight(const CatalogItem* a, int n, int w) {
    for (int i = 0; i < n; ++i) {
        if (a[i].weight == w) return i;
    }
    return -1;
}

int findItemByValue(const CatalogItem* a, int n, int v) {
    for (int i = 0; i < n; ++i) {
        if (a[i].value == v) return i;
    }
    return -1;
}

int binarySearchItemByWeight(const CatalogItem* a, int n, int w) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid].weight == w) return mid;
        if (a[mid].weight < w) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}
