// HashTable.cpp - Module A (Mohsina Zeeshan)
#include "core/HashTable.h"
#include <iostream>
#include <iomanip>

HashTable::HashTable(HashMode m, int initialCapacity, double maxLoadFactor)
    : mode(m), capacity(0), count(0), tombstones(0), maxLoad(maxLoadFactor),
      collisions(0), rehashCount(0), buckets(nullptr), slots(nullptr), states(nullptr) {
    allocate(nextPrime(initialCapacity < 3 ? 3 : initialCapacity));
}

HashTable::~HashTable() {
    release();
}

const char* HashTable::modeName(HashMode m) {
    switch (m) {
        case CHAINING:       return "Separate chaining";
        case LINEAR_PROBING: return "Linear probing";
        case DOUBLE_HASHING: return "Double hashing";
    }
    return "?";
}

// ---------- hashing ----------

// Polynomial rolling hash on the lowercase name: h = h * 31 + c
unsigned long HashTable::hashString(const String& key) const {
    unsigned long h = 0;
    for (int i = 0; i < key.length(); i++) {
        char c = key[i];
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
        h = h * 31 + (unsigned char)c;
    }
    return h;
}

int HashTable::hash1(unsigned long h) const {
    return (int)(h % capacity);
}

// Step size for double hashing. Always between 1 and capacity-1, and because
// capacity is prime, every step size visits every slot before repeating.
int HashTable::hash2(unsigned long h) const {
    return 1 + (int)((h / capacity) % (capacity - 1));
}

// i-th slot to try for this key
int HashTable::probeIndex(unsigned long h, int i) const {
    int step = (mode == DOUBLE_HASHING) ? hash2(h) : 1;
    return (int)((hash1(h) + (long long)i * step) % capacity);
}

// ---------- memory ----------

void HashTable::allocate(int cap) {
    capacity = cap;
    count = 0;
    tombstones = 0;
    if (mode == CHAINING) {
        buckets = new ChainNode*[capacity];
        for (int i = 0; i < capacity; i++) buckets[i] = nullptr;
    } else {
        slots = new Item[capacity];
        states = new SlotState[capacity];
        for (int i = 0; i < capacity; i++) states[i] = EMPTY;
    }
}

void HashTable::release() {
    if (buckets) {
        for (int i = 0; i < capacity; i++) {
            ChainNode* cur = buckets[i];
            while (cur) {
                ChainNode* next = cur->next;
                delete cur;
                cur = next;
            }
        }
        delete[] buckets;
        buckets = nullptr;
    }
    delete[] slots;
    delete[] states;
    slots = nullptr;
    states = nullptr;
}

void HashTable::clear() {
    release();
    allocate(capacity);
}

bool HashTable::isPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; d++)
        if (n % d == 0) return false;
    return true;
}

int HashTable::nextPrime(int n) {
    while (!isPrime(n)) n++;
    return n;
}

// ---------- rehashing ----------

// Copy every item out, make a table about twice as big (next prime),
// and insert everything again (positions change because capacity changed).
void HashTable::rehash() {
    DynArray<Item> all;
    toArray(all);
    long savedCollisions = collisions;   // don't count rehash work as collisions
    release();
    allocate(nextPrime(capacity * 2));
    for (int i = 0; i < all.size(); i++) insertInternal(all[i]);
    collisions = savedCollisions;
    rehashCount++;
}

// ---------- lookup ----------

int HashTable::findSlot(const String& key) const {
    unsigned long h = hashString(key);
    String lowerKey = key.toLower();
    for (int i = 0; i < capacity; i++) {
        int idx = probeIndex(h, i);
        if (states[idx] == EMPTY) return -1;            // empty slot ends the search
        if (states[idx] == OCCUPIED && slots[idx].name.toLower() == lowerKey) return idx;
        // DELETED: keep probing, the item may be further along
    }
    return -1;
}

Item* HashTable::findItem(const String& name) {
    if (mode == CHAINING) {
        String lowerKey = name.toLower();
        for (ChainNode* cur = buckets[hash1(hashString(name))]; cur; cur = cur->next)
            if (cur->item.name.toLower() == lowerKey) return &cur->item;
        return nullptr;
    }
    int idx = findSlot(name);
    return (idx == -1) ? nullptr : &slots[idx];
}

const Item* HashTable::findItem(const String& name) const {
    return const_cast<HashTable*>(this)->findItem(name);
}

// ---------- insert / remove ----------

void HashTable::insertInternal(const Item& item) {
    unsigned long h = hashString(item.name);
    if (mode == CHAINING) {
        int b = hash1(h);
        if (buckets[b] != nullptr) collisions++;        // bucket already in use
        buckets[b] = new ChainNode(item, buckets[b]);   // insert at head of chain
        count++;
        return;
    }
    for (int i = 0; i < capacity; i++) {
        int idx = probeIndex(h, i);
        if (states[idx] != OCCUPIED) {
            if (states[idx] == DELETED) tombstones--;   // reuse the tombstone
            slots[idx] = item;
            states[idx] = OCCUPIED;
            count++;
            return;
        }
        collisions++;                                    // slot taken, probe again
    }
}

bool HashTable::addItem(const Item& item) {
    Item* existing = findItem(item.name);
    if (existing) {
        existing->quantity += item.quantity;
        return false;
    }
    // Grow before the table gets too full (tombstones also slow probing down).
    if ((double)(count + tombstones + 1) / capacity > maxLoad) rehash();
    insertInternal(item);
    return true;
}

bool HashTable::removeItem(const String& name, int amount) {
    Item* it = findItem(name);
    if (!it) return false;
    if (amount > 0 && it->quantity > amount) {           // just reduce the quantity
        it->quantity -= amount;
        return true;
    }
    // remove the whole entry
    if (mode == CHAINING) {
        String lowerKey = name.toLower();
        int b = hash1(hashString(name));
        ChainNode* prev = nullptr;
        ChainNode* cur = buckets[b];
        while (cur && !(cur->item.name.toLower() == lowerKey)) {
            prev = cur;
            cur = cur->next;
        }
        if (prev) prev->next = cur->next; else buckets[b] = cur->next;
        delete cur;
    } else {
        // Can't just mark EMPTY: that would cut off items that probed past this slot.
        int idx = findSlot(name);
        states[idx] = DELETED;
        slots[idx] = Item();
        tombstones++;
    }
    count--;
    return true;
}

// ---------- listing ----------

void HashTable::toArray(DynArray<Item>& out) const {
    out.clear();
    if (mode == CHAINING) {
        for (int i = 0; i < capacity; i++)
            for (ChainNode* cur = buckets[i]; cur; cur = cur->next) out.pushBack(cur->item);
    } else {
        for (int i = 0; i < capacity; i++)
            if (states[i] == OCCUPIED) out.pushBack(slots[i]);
    }
}

void HashTable::listItems() const {
    if (count == 0) {
        std::cout << "Your inventory is empty.\n";
        return;
    }
    DynArray<Item> all;
    toArray(all);
    std::cout << std::left << std::setw(20) << "Item" << std::setw(8) << "Qty"
              << std::setw(8) << "Weight" << "Value\n";
    for (int i = 0; i < all.size(); i++) {
        std::cout << std::setw(20) << all[i].name.c_str() << std::setw(8) << all[i].quantity
                  << std::setw(8) << all[i].weight << all[i].value << "\n";
    }
    std::cout << std::right;
}
