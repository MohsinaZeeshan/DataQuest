// StringMatch.h - Module A (Mohsina Zeeshan)
// Four string-matching algorithms with the SAME signature, so the benchmark
// and the game-log search can call any of them through a function pointer.
// Each returns how many times pattern occurs in text, and stores the
// starting index of every match in `positions`.
//
//   n = text length, m = pattern length
//   Brute force : O(n*m) worst
//   Rabin-Karp  : O(n+m) average, O(n*m) worst (many hash collisions)
//   Boyer-Moore : O(n/m) best, O(n*m) worst (bad-character rule only)
//   KMP         : O(n+m) always, O(m) extra space for the prefix table
#ifndef DATAQUEST_STRINGMATCH_H
#define DATAQUEST_STRINGMATCH_H

#include "String.h"
#include "DynArray.h"

enum MatchAlgorithm { BRUTE_FORCE, RABIN_KARP, BOYER_MOORE, KMP };

typedef int (*MatchFunction)(const String& text, const String& pattern, DynArray<int>& positions);

int bruteForceSearch(const String& text, const String& pattern, DynArray<int>& positions);
int rabinKarpSearch(const String& text, const String& pattern, DynArray<int>& positions);
int boyerMooreSearch(const String& text, const String& pattern, DynArray<int>& positions);
int kmpSearch(const String& text, const String& pattern, DynArray<int>& positions);

// KMP prefix table (also called LPS / failure function).
// lps[i] = length of the longest proper prefix of pattern[0..i] that is also a suffix.
// `lps` must have room for pattern.length() ints.
void buildPrefixTable(const String& pattern, int* lps);
void printPrefixTable(const String& pattern);   // for the viva / report

MatchFunction getMatcher(MatchAlgorithm algo);
const char* matcherName(MatchAlgorithm algo);

#endif
