// Search.h - searching algorithms (Module B).
//  linear search        O(n)            works on unsorted data / linked lists
//  binary search        O(log n)        sorted array
//  interpolation search O(log log n) avg on uniform data, O(n) worst; sorted array
// All array searches return the index, or -1 if not found.
// Plain functions on int arrays - no templates.
#ifndef TREES_SEARCH_H
#define TREES_SEARCH_H

#include "Item.h"

int linearSearch(const int* a, int n, int key);
int binarySearch(const int* a, int n, int key);
int binarySearchRec(const int* a, int lo, int hi, int key);
int interpolationSearch(const int* a, int n, int key);

// Singly linked list node used by linearSearchList.
// (Rename the fields to match Module A's LinkedList node if it differs.)
struct ListNode {
    int data;
    ListNode* next;
};

// Linear search on a singly linked list. Returns the node, or 0.
ListNode* linearSearchList(ListNode* head, int key);

// Item search (linear, unsorted inventory).
int findItemByName(const Item* a, int n, const char* name);
int findItemByWeight(const Item* a, int n, int w);
int findItemByValue(const Item* a, int n, int v);

// Binary search on an array already sorted by weight
// (call sortItemsByWeight first).
int binarySearchItemByWeight(const Item* a, int n, int w);

#endif
