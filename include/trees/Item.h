// Item.h - the item record used by the catalog, search and inventory sorting.
// Named CatalogItem (not Item) so it does not clash with Module A's Item
// in core/HashTable.h when all modules are built together.
// char arrays are used so Module B builds alone. If Module A freezes its
// String class first, swap `char name[32]` for that String type and
// adjust the strcmp calls in the .cpp files.
#ifndef TREES_ITEM_H
#define TREES_ITEM_H

struct CatalogItem {
    int id;
    char name[32];
    int weight;
    int value;

    CatalogItem();
    CatalogItem(int i, const char* n, int w, int v);
};

// Swap two items (used by the item sorts).
void swapItems(CatalogItem& a, CatalogItem& b);

// Copy a name safely into a 32-char buffer (always null-terminated).
void copyItemName(char dest[32], const char* src);

#endif
