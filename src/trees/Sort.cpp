// Sort.cpp - implementation of the sorts in Sort.h.
#include "Sort.h"
#include "Common.h"

// ---------------- Simple O(n^2) sorts ----------------
void bubbleSort(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (a[j + 1] < a[j]) {
                swapInt(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;  // already sorted -> O(n) best case
    }
}

void selectionSort(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int m = i;
        for (int j = i + 1; j < n; ++j) {
            if (a[j] < a[m]) m = j;
        }
        if (m != i) swapInt(a[i], a[m]);
    }
}

void insertionSort(int* a, int n) {
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && key < a[j]) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

void combSort(int* a, int n) {
    int gap = n;
    bool sorted = false;
    while (!sorted) {
        gap = (gap * 10) / 13;  // shrink factor ~1.3
        if (gap <= 1) {
            gap = 1;
            sorted = true;
        }
        for (int i = 0; i + gap < n; ++i) {
            if (a[i + gap] < a[i]) {
                swapInt(a[i], a[i + gap]);
                sorted = false;
            }
        }
    }
}

// ---------------- Radix sort (LSD, base 10, handles negatives via offset) ----------------
void radixSort(int* a, int n) {
    if (n < 2) return;
    int mn = a[0], mx = a[0];
    for (int i = 1; i < n; ++i) {
        if (a[i] < mn) mn = a[i];
        if (a[i] > mx) mx = a[i];
    }
    long long range = (long long)mx - mn;
    int* out = new int[n];
    for (long long exp = 1; range / exp > 0; exp *= 10) {
        int cnt[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < n; ++i) cnt[((a[i] - mn) / exp) % 10]++;
        for (int d = 1; d < 10; ++d) cnt[d] += cnt[d - 1];
        for (int i = n - 1; i >= 0; --i) {  // backwards keeps it stable
            int d = (int)(((a[i] - mn) / exp) % 10);
            out[--cnt[d]] = a[i];
        }
        for (int i = 0; i < n; ++i) a[i] = out[i];
    }
    delete[] out;
}

// ---------------- Merge sort (stable) ----------------
static void mergeRec(int* a, int* tmp, int lo, int hi) {  // sorts [lo, hi)
    if (hi - lo < 2) return;
    int mid = lo + (hi - lo) / 2;
    mergeRec(a, tmp, lo, mid);
    mergeRec(a, tmp, mid, hi);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        if (a[j] < a[i]) tmp[k++] = a[j++];  // take left on ties -> stable
        else tmp[k++] = a[i++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (k = lo; k < hi; ++k) a[k] = tmp[k];
}

void mergeSort(int* a, int n) {
    if (n < 2) return;
    int* tmp = new int[n];
    mergeRec(a, tmp, 0, n);
    delete[] tmp;
}

// ---------------- Quick sort (Hoare partition, median-of-three) ----------------
static void quickRec(int* a, int lo, int hi) {  // sorts [lo, hi] inclusive
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] < a[lo]) swapInt(a[mid], a[lo]);
        if (a[hi] < a[lo]) swapInt(a[hi], a[lo]);
        if (a[hi] < a[mid]) swapInt(a[hi], a[mid]);
        int pivot = a[mid];
        int i = lo, j = hi;
        while (i <= j) {
            while (a[i] < pivot) ++i;
            while (pivot < a[j]) --j;
            if (i <= j) {
                swapInt(a[i], a[j]);
                ++i;
                --j;
            }
        }
        // Recurse on the smaller part, loop on the larger -> stack O(log n).
        if (j - lo < hi - i) {
            quickRec(a, lo, j);
            lo = i;
        } else {
            quickRec(a, i, hi);
            hi = j;
        }
    }
}

void quickSort(int* a, int n) {
    if (n > 1) quickRec(a, 0, n - 1);
}

// ---------------- Inventory sorting (Items) ----------------
// byWeight: 0 = weight, 1 = value. Shared merge / quick helpers.
static void mergeItemsRec(Item* a, Item* tmp, int lo, int hi, int byWeight) {
    if (hi - lo < 2) return;
    int mid = lo + (hi - lo) / 2;
    mergeItemsRec(a, tmp, lo, mid, byWeight);
    mergeItemsRec(a, tmp, mid, hi, byWeight);
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        int leftKey = byWeight ? a[i].weight : a[i].value;
        int rightKey = byWeight ? a[j].weight : a[j].value;
        if (rightKey < leftKey) tmp[k++] = a[j++];
        else tmp[k++] = a[i++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi) tmp[k++] = a[j++];
    for (k = lo; k < hi; ++k) a[k] = tmp[k];
}

static void mergeItemsBy(Item* a, int n, int byWeight) {
    if (n < 2) return;
    Item* tmp = new Item[n];
    mergeItemsRec(a, tmp, 0, n, byWeight);
    delete[] tmp;
}

static void quickItemsRec(Item* a, int lo, int hi, int byWeight) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        int keyLo = byWeight ? a[lo].weight : a[lo].value;
        int keyMid = byWeight ? a[mid].weight : a[mid].value;
        int keyHi = byWeight ? a[hi].weight : a[hi].value;
        if (keyMid < keyLo) swapItems(a[mid], a[lo]);
        if (keyHi < keyLo) swapItems(a[hi], a[lo]);
        if (keyHi < keyMid) swapItems(a[hi], a[mid]);
        Item pivot = a[mid];
        int pivotKey = byWeight ? pivot.weight : pivot.value;
        int i = lo, j = hi;
        while (i <= j) {
            while ((byWeight ? a[i].weight : a[i].value) < pivotKey) ++i;
            while (pivotKey < (byWeight ? a[j].weight : a[j].value)) --j;
            if (i <= j) {
                swapItems(a[i], a[j]);
                ++i;
                --j;
            }
        }
        if (j - lo < hi - i) {
            quickItemsRec(a, lo, j, byWeight);
            lo = i;
        } else {
            quickItemsRec(a, i, hi, byWeight);
            hi = j;
        }
    }
}

void sortItemsByWeight(Item* a, int n) { mergeItemsBy(a, n, 1); }  // stable
void sortItemsByValue(Item* a, int n) { mergeItemsBy(a, n, 0); }
void sortItemsByWeightQuick(Item* a, int n) {
    if (n > 1) quickItemsRec(a, 0, n - 1, 1);
}

void sortItemsByValueRadix(Item* a, int n) {
    if (n < 2) return;
    int mn = a[0].value, mx = a[0].value;
    for (int i = 1; i < n; ++i) {
        if (a[i].value < mn) mn = a[i].value;
        if (a[i].value > mx) mx = a[i].value;
    }
    long long range = (long long)mx - mn;
    Item* out = new Item[n];
    for (long long exp = 1; range / exp > 0; exp *= 10) {
        int cnt[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (int i = 0; i < n; ++i) cnt[((a[i].value - mn) / exp) % 10]++;
        for (int d = 1; d < 10; ++d) cnt[d] += cnt[d - 1];
        for (int i = n - 1; i >= 0; --i) {
            int d = (int)(((a[i].value - mn) / exp) % 10);
            out[--cnt[d]] = a[i];
        }
        for (int i = 0; i < n; ++i) a[i] = out[i];
    }
    delete[] out;
}
