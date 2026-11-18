// String.h - Module A (Mohsina Zeeshan)
// Our own string class. Stores characters in a heap array that we manage
// ourselves (deep copy on copy/assign, freed in the destructor).
#ifndef DATAQUEST_STRING_H
#define DATAQUEST_STRING_H

#include <iostream>

template <typename T> class DynArray;   // defined in DynArray.h

class String {
private:
    char* data;   // null-terminated character array on the heap
    int len;      // number of characters (without the '\0')

    static int cstrLength(const char* s);

public:
    // ----- rule of three -----
    String();                              // empty string            O(1)
    String(const char* s);                 // from a C string          O(n)
    String(char c);                        // one character            O(1)
    String(const String& other);           // deep copy                O(n)
    String& operator=(const String& other);// deep copy assignment     O(n)
    ~String();                             // frees data               O(1)

    // ----- basic access -----
    int length() const { return len; }
    bool empty() const { return len == 0; }
    const char* c_str() const { return data; }
    char operator[](int i) const;          // bounds-checked read
    char& operator[](int i);               // bounds-checked write

    // ----- operations from the task list -----
    String substr(int start, int count = -1) const;  // count -1 = to the end   O(n)
    int compare(const String& other) const;          // -1, 0 or 1 (like strcmp) O(n)
    String operator+(const String& other) const;     // concatenation            O(n+m)
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const { return !(*this == other); }
    bool operator<(const String& other) const  { return compare(other) < 0; }
    bool operator>(const String& other) const  { return compare(other) > 0; }

    // ----- helpers used by the parser, save/load and the game -----
    String& operator+=(const String& other);
    String& operator+=(char c);
    int find(char c, int from = 0) const;            // index or -1
    int find(const String& sub, int from = 0) const; // index or -1 (brute force)
    String toLower() const;
    String trim() const;                             // remove spaces at both ends
    bool toInt(int& out) const;                      // false if not a valid integer
    static String fromInt(int value);
    // Split on a delimiter. With skipEmpty, repeated delimiters are ignored.
    void split(char delim, DynArray<String>& out, bool skipEmpty = true) const;

    // Reads one line (strips '\r' and '\n'). Returns false at end of file.
    static bool readLine(std::istream& in, String& out);

    friend std::ostream& operator<<(std::ostream& os, const String& s);
};

#endif
