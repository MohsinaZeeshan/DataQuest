// Item.cpp - implementation of the Item record in Item.h.
#include "Item.h"
#include <cstring>

Item::Item() : id(0), weight(0), value(0) {
    name[0] = '\0';
}

Item::Item(int i, const char* n, int w, int v) : id(i), weight(w), value(v) {
    copyItemName(name, n);
}

void swapItems(Item& a, Item& b) {
    Item t = a;
    a = b;
    b = t;
}

void copyItemName(char dest[32], const char* src) {
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, 31);
    dest[31] = '\0';
}
