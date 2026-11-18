// String.cpp - Module A (Mohsina Zeeshan)
#include "core/String.h"
#include "core/DynArray.h"
#include <stdexcept>

int String::cstrLength(const char* s) {
    int n = 0;
    while (s[n] != '\0') n++;
    return n;
}

String::String() : data(new char[1]), len(0) {
    data[0] = '\0';
}

String::String(const char* s) {
    if (s == nullptr) s = "";
    len = cstrLength(s);
    data = new char[len + 1];
    for (int i = 0; i <= len; i++) data[i] = s[i];   // copies the '\0' too
}

String::String(char c) : data(new char[2]), len(1) {
    data[0] = c;
    data[1] = '\0';
}

// Deep copy: new memory, then copy every character.
// (A shallow copy would share one array and delete it twice.)
String::String(const String& other) : data(new char[other.len + 1]), len(other.len) {
    for (int i = 0; i <= len; i++) data[i] = other.data[i];
}

String& String::operator=(const String& other) {
    if (this == &other) return *this;           // a = a must not delete itself
    char* fresh = new char[other.len + 1];      // allocate first (safe if new fails)
    for (int i = 0; i <= other.len; i++) fresh[i] = other.data[i];
    delete[] data;
    data = fresh;
    len = other.len;
    return *this;
}

String::~String() {
    delete[] data;
}

char String::operator[](int i) const {
    if (i < 0 || i >= len) throw std::out_of_range("String index out of range");
    return data[i];
}

char& String::operator[](int i) {
    if (i < 0 || i >= len) throw std::out_of_range("String index out of range");
    return data[i];
}

String String::substr(int start, int count) const {
    if (start < 0 || start >= len) return String();
    if (count < 0 || start + count > len) count = len - start;
    char* buf = new char[count + 1];
    for (int i = 0; i < count; i++) buf[i] = data[start + i];
    buf[count] = '\0';
    String result(buf);
    delete[] buf;
    return result;
}

int String::compare(const String& other) const {
    int i = 0;
    while (i < len && i < other.len) {
        if (data[i] < other.data[i]) return -1;
        if (data[i] > other.data[i]) return 1;
        i++;
    }
    if (len < other.len) return -1;   // "cat" < "cats"
    if (len > other.len) return 1;
    return 0;
}

String String::operator+(const String& other) const {
    String result;
    delete[] result.data;
    result.len = len + other.len;
    result.data = new char[result.len + 1];
    for (int i = 0; i < len; i++) result.data[i] = data[i];
    for (int i = 0; i <= other.len; i++) result.data[len + i] = other.data[i];
    return result;
}

bool String::operator==(const String& other) const {
    if (len != other.len) return false;
    for (int i = 0; i < len; i++)
        if (data[i] != other.data[i]) return false;
    return true;
}

String& String::operator+=(const String& other) {
    *this = *this + other;
    return *this;
}

String& String::operator+=(char c) {
    char* fresh = new char[len + 2];
    for (int i = 0; i < len; i++) fresh[i] = data[i];
    fresh[len] = c;
    fresh[len + 1] = '\0';
    delete[] data;
    data = fresh;
    len++;
    return *this;
}

int String::find(char c, int from) const {
    for (int i = (from < 0 ? 0 : from); i < len; i++)
        if (data[i] == c) return i;
    return -1;
}

int String::find(const String& sub, int from) const {
    if (from < 0) from = 0;
    for (int i = from; i + sub.len <= len; i++) {
        int j = 0;
        while (j < sub.len && data[i + j] == sub.data[j]) j++;
        if (j == sub.len) return i;
    }
    return -1;
}

String String::toLower() const {
    String result(*this);
    for (int i = 0; i < len; i++)
        if (result.data[i] >= 'A' && result.data[i] <= 'Z')
            result.data[i] = result.data[i] - 'A' + 'a';
    return result;
}

String String::trim() const {
    int start = 0, end = len - 1;
    while (start < len && (data[start] == ' ' || data[start] == '\t')) start++;
    while (end >= start && (data[end] == ' ' || data[end] == '\t')) end--;
    return substr(start, end - start + 1);
}

bool String::toInt(int& out) const {
    if (len == 0) return false;
    int i = 0;
    bool negative = false;
    if (data[0] == '-' || data[0] == '+') {
        negative = (data[0] == '-');
        i = 1;
        if (len == 1) return false;
    }
    long long value = 0;
    for (; i < len; i++) {
        if (data[i] < '0' || data[i] > '9') return false;
        value = value * 10 + (data[i] - '0');
        if (value > 2147483647LL) return false;    // would overflow an int
    }
    out = (int)(negative ? -value : value);
    return true;
}

String String::fromInt(int value) {
    if (value == 0) return String("0");
    bool negative = value < 0;
    long long v = value;
    if (negative) v = -v;
    char buf[16];
    int pos = 15;
    buf[pos] = '\0';
    while (v > 0) {
        buf[--pos] = (char)('0' + v % 10);
        v /= 10;
    }
    if (negative) buf[--pos] = '-';
    return String(buf + pos);
}

void String::split(char delim, DynArray<String>& out, bool skipEmpty) const {
    out.clear();
    String piece;
    for (int i = 0; i < len; i++) {
        if (data[i] == delim) {
            if (!skipEmpty || !piece.empty()) out.pushBack(piece);
            piece = String();
        } else {
            piece += data[i];
        }
    }
    if (!skipEmpty || !piece.empty()) out.pushBack(piece);
}

bool String::readLine(std::istream& in, String& out) {
    out = String();
    char c;
    bool readAnything = false;
    while (in.get(c)) {
        readAnything = true;
        if (c == '\n') break;
        if (c != '\r') out += c;
    }
    return readAnything;
}

std::ostream& operator<<(std::ostream& os, const String& s) {
    os << s.data;
    return os;
}
