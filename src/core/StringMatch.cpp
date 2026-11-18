// StringMatch.cpp - Module A (Mohsina Zeeshan)
#include "core/StringMatch.h"
#include <iostream>

// ---------- Brute force ----------
// Try every starting position and compare character by character.
int bruteForceSearch(const String& text, const String& pattern, DynArray<int>& positions) {
    positions.clear();
    int n = text.length(), m = pattern.length();
    if (m == 0 || m > n) return 0;
    const char* t = text.c_str();
    const char* p = pattern.c_str();
    for (int s = 0; s <= n - m; s++) {
        int j = 0;
        while (j < m && t[s + j] == p[j]) j++;
        if (j == m) positions.pushBack(s);
    }
    return positions.size();
}

// ---------- Rabin-Karp ----------
// Compare hashes first; only compare characters when the hashes are equal.
// The window hash is updated in O(1) by removing the left char and adding the right one.
int rabinKarpSearch(const String& text, const String& pattern, DynArray<int>& positions) {
    positions.clear();
    int n = text.length(), m = pattern.length();
    if (m == 0 || m > n) return 0;
    const char* t = text.c_str();
    const char* p = pattern.c_str();
    const long long d = 256;        // alphabet size
    const long long q = 1000003;    // a large prime modulus

    long long h = 1;                // d^(m-1) mod q, weight of the leftmost char
    for (int i = 0; i < m - 1; i++) h = (h * d) % q;

    long long pHash = 0, tHash = 0;
    for (int i = 0; i < m; i++) {
        pHash = (d * pHash + (unsigned char)p[i]) % q;
        tHash = (d * tHash + (unsigned char)t[i]) % q;
    }

    for (int s = 0; s <= n - m; s++) {
        if (pHash == tHash) {                       // possible match: verify it
            int j = 0;
            while (j < m && t[s + j] == p[j]) j++;
            if (j == m) positions.pushBack(s);
        }
        if (s < n - m) {                            // roll the window one step right
            tHash = (d * (tHash - (unsigned char)t[s] * h) + (unsigned char)t[s + m]) % q;
            if (tHash < 0) tHash += q;
        }
    }
    return positions.size();
}

// ---------- Boyer-Moore (bad-character rule) ----------
// Compare from the END of the pattern. On a mismatch, shift so the bad text
// character lines up with its last occurrence in the pattern.
int boyerMooreSearch(const String& text, const String& pattern, DynArray<int>& positions) {
    positions.clear();
    int n = text.length(), m = pattern.length();
    if (m == 0 || m > n) return 0;
    const char* t = text.c_str();
    const char* p = pattern.c_str();

    int last[256];                                  // last index of each char in pattern
    for (int i = 0; i < 256; i++) last[i] = -1;
    for (int i = 0; i < m; i++) last[(unsigned char)p[i]] = i;

    int s = 0;
    while (s <= n - m) {
        int j = m - 1;
        while (j >= 0 && p[j] == t[s + j]) j--;
        if (j < 0) {
            positions.pushBack(s);
            // shift so the next text char lines up with its last occurrence
            s += (s + m < n) ? m - last[(unsigned char)t[s + m]] : 1;
        } else {
            int shift = j - last[(unsigned char)t[s + j]];
            s += (shift > 1) ? shift : 1;
        }
    }
    return positions.size();
}

// ---------- KMP ----------
void buildPrefixTable(const String& pattern, int* lps) {
    int m = pattern.length();
    if (m == 0) return;
    const char* p = pattern.c_str();
    lps[0] = 0;
    int len = 0;           // length of the current matching prefix
    int i = 1;
    while (i < m) {
        if (p[i] == p[len]) {
            len++;
            lps[i] = len;
            i++;
        } else if (len > 0) {
            len = lps[len - 1];   // fall back to a shorter prefix, don't move i
        } else {
            lps[i] = 0;
            i++;
        }
    }
}

void printPrefixTable(const String& pattern) {
    int m = pattern.length();
    if (m == 0) return;
    int* lps = new int[m];
    buildPrefixTable(pattern, lps);
    std::cout << "index : ";
    for (int i = 0; i < m; i++) std::cout << i << " ";
    std::cout << "\nchar  : ";
    for (int i = 0; i < m; i++) std::cout << pattern[i] << " ";
    std::cout << "\nlps   : ";
    for (int i = 0; i < m; i++) std::cout << lps[i] << " ";
    std::cout << "\n";
    delete[] lps;
}

// The text pointer never moves backwards; on a mismatch the pattern
// jumps using the prefix table instead of starting over.
int kmpSearch(const String& text, const String& pattern, DynArray<int>& positions) {
    positions.clear();
    int n = text.length(), m = pattern.length();
    if (m == 0 || m > n) return 0;
    const char* t = text.c_str();
    const char* p = pattern.c_str();
    int* lps = new int[m];
    buildPrefixTable(pattern, lps);

    int i = 0, j = 0;      // i in text, j in pattern
    while (i < n) {
        if (t[i] == p[j]) {
            i++;
            j++;
            if (j == m) {
                positions.pushBack(i - m);
                j = lps[j - 1];         // keep going to find overlapping matches
            }
        } else if (j > 0) {
            j = lps[j - 1];
        } else {
            i++;
        }
    }
    delete[] lps;
    return positions.size();
}

// ---------- helpers ----------
MatchFunction getMatcher(MatchAlgorithm algo) {
    switch (algo) {
        case BRUTE_FORCE: return bruteForceSearch;
        case RABIN_KARP:  return rabinKarpSearch;
        case BOYER_MOORE: return boyerMooreSearch;
        case KMP:         return kmpSearch;
    }
    return bruteForceSearch;
}

const char* matcherName(MatchAlgorithm algo) {
    switch (algo) {
        case BRUTE_FORCE: return "Brute force";
        case RABIN_KARP:  return "Rabin-Karp";
        case BOYER_MOORE: return "Boyer-Moore";
        case KMP:         return "KMP";
    }
    return "?";
}
