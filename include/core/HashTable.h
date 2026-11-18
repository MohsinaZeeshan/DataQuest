// HashTable.h - Module A (Mohsina Zeeshan)
// Inventory hash table: item name -> Item.
// Three collision strategies: separate chaining, linear probing, double hashing.
// Rehashes automatically when the load factor passes maxLoadFactor (0.7).
// Names are case-insensitive ("Sword" and "sword" are the same item).
//
// Average: add / remove / find O(1)   Worst case: O(n)   Rehash: O(n)
#ifndef DATAQUEST_HASHTABLE_H
#define DATAQUEST_HASHTABLE_H

#include "String.h"
#include "DynArray.h"

struct Item {
    String name;
    int id;         // unique item ID (Module B's catalog can key on this)
    int weight;
    int value;
    int quantity;

    Item() : id(0), weight(0), value(0), quantity(0) {}
    Item(const String& n, int w, int v, int q = 1, int itemId = 0)
        : name(n), id(itemId), weight(w), value(v), quantity(q) {}
};

enum HashMode { CHAINING, LINEAR_PROBING, DOUBLE_HASHING };

class HashTable {
public:
    HashTable(HashMode mode = CHAINING, int initialCapacity = 11, double maxLoadFactor = 0.7);
    ~HashTable();
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Inventory functions
    // addItem: if the item already exists its quantity goes up. Returns true if it was new.
    bool addItem(const Item& item);
    // removeItem: takes away `amount` units (-1 = all). Returns false if not found.
    bool removeItem(const String& name, int amount = -1);
    Item* findItem(const String& name);              // nullptr if not found
    const Item* findItem(const String& name) const;
    void listItems() const;                          // prints the inventory
    void toArray(DynArray<Item>& out) const;         // copy items out (for sorting / saving)
    void clear();

    // Stats for the benchmark and report
    int size() const { return count; }
    int getCapacity() const { return capacity; }
    double loadFactor() const { return (double)count / capacity; }
    long getCollisions() const { return collisions; }
    void resetCollisions() { collisions = 0; }
    int getRehashCount() const { return rehashCount; }
    HashMode getMode() const { return mode; }
    static const char* modeName(HashMode m);

private:
    struct ChainNode {
        Item item;
        ChainNode* next;
        ChainNode(const Item& it, ChainNode* n) : item(it), next(n) {}
    };
    enum SlotState { EMPTY, OCCUPIED, DELETED };   // DELETED = tombstone

    HashMode mode;
    int capacity;          // always a prime number
    int count;             // items stored
    int tombstones;        // DELETED slots (open addressing only)
    double maxLoad;
    long collisions;
    int rehashCount;

    ChainNode** buckets;   // chaining
    Item* slots;           // open addressing
    SlotState* states;     // open addressing

    unsigned long hashString(const String& key) const;
    int hash1(unsigned long h) const;
    int hash2(unsigned long h) const;
    int probeIndex(unsigned long h, int i) const;
    int findSlot(const String& key) const;          // open addressing: index or -1
    void insertInternal(const Item& item);          // no duplicate / rehash checks
    void allocate(int cap);
    void release();
    void rehash();
    static bool isPrime(int n);
    static int nextPrime(int n);
};

#endif
