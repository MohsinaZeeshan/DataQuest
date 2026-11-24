// Sort.h - all sorting algorithms (Module B).
// Every plain sort has the SAME signature:  void xSort(int* a, int n)
// so the benchmark can hold them in one table of SortFn.
//
//  Algorithm   Best       Average     Worst       Space   Stable
//  bubble      O(n)       O(n^2)      O(n^2)      O(1)    yes
//  selection   O(n^2)     O(n^2)      O(n^2)      O(1)    no
//  insertion   O(n)       O(n^2)      O(n^2)      O(1)    yes
//  comb        O(n log n) ~O(n^2/2^p) O(n^2)      O(1)    no
//  radix       O(d(n+b))  O(d(n+b))   O(d(n+b))   O(n+b)  yes
//  merge       O(n log n) O(n log n)  O(n log n)  O(n)    yes
//  quick       O(n log n) O(n log n)  O(n^2)      O(log n) no
//  heap        O(n log n) O(n log n)  O(n log n)  O(1)    no (see Heap.h)
//
// All sorts here are normal functions on plain int arrays - no templates.
#ifndef TREES_SORT_H
#define TREES_SORT_H

#include "Item.h"

typedef void (*SortFn)(int*, int);

void bubbleSort(int* a, int n);
void selectionSort(int* a, int n);
void insertionSort(int* a, int n);
void combSort(int* a, int n);
void radixSort(int* a, int n);
void mergeSort(int* a, int n);
void quickSort(int* a, int n);

// Inventory sorting (Items). Weight / value sorts are stable merge sorts
// unless the name says Quick / Radix.
void sortItemsByWeight(CatalogItem* a, int n);
void sortItemsByValue(CatalogItem* a, int n);
void sortItemsByWeightQuick(CatalogItem* a, int n);
void sortItemsByValueRadix(CatalogItem* a, int n);

#endif
