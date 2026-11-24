// test_core.cpp - Module A tests (Mohsina Zeeshan)
// Build and run with:  make test
// Keep this output for the report.
#include "core/String.h"
#include "core/DynArray.h"
#include "core/LinkedList.h"
#include "core/Stack.h"
#include "core/Queue.h"
#include "core/HashTable.h"
#include "core/StringMatch.h"
#include "core/Parser.h"
#include "core/History.h"
#include "core/SaveLoad.h"
#include "core/Benchmark.h"
#include "core/Recursion.h"
#include "core/CoreBenchmarks.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

static int passed = 0, failed = 0;

#define CHECK(cond, msg)                                         \
    do {                                                         \
        if (cond) passed++;                                      \
        else { failed++; std::cout << "  FAIL: " << msg << "\n"; } \
    } while (0)

static void section(const char* name) { std::cout << "\n--- " << name << " ---\n"; }

// ------------------------------------------------------------
static void testString() {
    section("String");
    String a("Hello"), b(" World");
    String c = a + b;
    CHECK(c == "Hello World", "concatenation");
    CHECK(c.length() == 11, "length");
    CHECK(c.substr(6, 5) == "World", "substr");
    CHECK(c.substr(6) == "World", "substr to end");
    CHECK(a.compare("Hellp") < 0 && a.compare("Hell") > 0 && a.compare("Hello") == 0, "compare");

    String copy(a);              // copy constructor
    copy[0] = 'J';
    CHECK(a == "Hello" && copy == "Jello", "deep copy (copy constructor)");
    String assigned;
    assigned = a;                // assignment operator
    assigned[0] = 'C';
    CHECK(a == "Hello" && assigned == "Cello", "deep copy (assignment)");
    assigned = assigned;         // self-assignment must be safe
    CHECK(assigned == "Cello", "self assignment");

    CHECK(String("  Pick Up  ").trim().toLower() == "pick up", "trim + toLower");
    int n = 0;
    CHECK(String("-42").toInt(n) && n == -42, "toInt");
    CHECK(!String("4x2").toInt(n), "toInt rejects bad input");
    CHECK(String::fromInt(-305) == "-305" && String::fromInt(0) == "0", "fromInt");
    DynArray<String> parts;
    String("a|b||c").split('|', parts, false);
    CHECK(parts.size() == 4 && parts[2] == "", "split keeps empty");
    String("  go   north ").split(' ', parts);
    CHECK(parts.size() == 2 && parts[1] == "north", "split skips empty");
    std::cout << "c = \"" << c << "\"\n";
}

static void testArrays() {
    section("DynArray / Array2D / JaggedArray");
    DynArray<int> arr;
    for (int i = 0; i < 10; i++) arr.pushBack(i * i);
    CHECK(arr.size() == 10 && arr[9] == 81, "pushBack");
    CHECK(arr.capacity() == 16, "capacity doubles 4 -> 8 -> 16");
    arr.insertAt(0, -1);
    arr.removeAt(5);
    CHECK(arr[0] == -1 && arr[5] == 25 && arr.size() == 10, "insertAt / removeAt");
    DynArray<int> arr2 = arr;
    arr2[0] = 100;
    CHECK(arr[0] == -1, "DynArray deep copy");
    bool threw = false;
    try { arr[50]; } catch (const std::out_of_range&) { threw = true; }
    CHECK(threw, "bounds check throws");

    Array2D<char> grid(20, 20, '#');
    grid.at(3, 4) = '@';
    CHECK(grid.at(3, 4) == '@' && grid.at(0, 0) == '#', "2D get/set");
    CHECK(grid.inBounds(19, 19) && !grid.inBounds(20, 0), "2D inBounds");
    Array2D<char> gridCopy = grid;
    gridCopy.at(3, 4) = '.';
    CHECK(grid.at(3, 4) == '@', "2D deep copy");

    JaggedArray<String> roomItems(3);
    roomItems.addToRow(0, "sword");
    roomItems.addToRow(0, "potion");
    roomItems.addToRow(2, "key");
    CHECK(roomItems.rowSize(0) == 2 && roomItems.rowSize(1) == 0 && roomItems.rowSize(2) == 1, "jagged row sizes");
    CHECK(roomItems.findInRow(0, "potion") == 1, "jagged find");
    roomItems.removeFromRow(0, 0);
    CHECK(roomItems.get(0, 0) == "potion", "jagged remove");
}

static void testLists() {
    section("Linked lists");
    SinglyLinkedList<int> s;
    s.insertTail(2); s.insertTail(3); s.insertHead(1); s.insertAt(3, 4); s.insertAt(1, 9);
    std::cout << "singly:   "; s.print();
    CHECK(s.size() == 5 && s.search(9) == 1, "singly insert + search");
    CHECK(s.deleteValue(9) && !s.deleteValue(42), "singly deleteValue");
    s.reverse();
    std::cout << "reversed: "; s.print();
    CHECK(s.get(0) == 4 && s.get(3) == 1, "singly reverse");
    s.deleteAt(0);
    s.insertTail(7);
    CHECK(s.get(3) == 7, "tail correct after reverse + delete");

    DoublyLinkedList<int> d;
    d.pushBack(2); d.pushBack(3); d.pushFront(1);
    std::cout << "forward:  "; d.printForward();
    std::cout << "backward: "; d.printBackward();
    CHECK(d.popBack() == 3 && d.popFront() == 1 && d.size() == 1, "doubly push/pop");

    CircularList<String> turns;
    turns.insert("Player"); turns.insert("Goblin"); turns.insert("Orc");
    std::cout << "turns:    "; turns.print();
    CHECK(turns.current() == "Player", "circular current");
    turns.next(); turns.next();
    CHECK(turns.next() == "Player", "circular wraps around");
    turns.remove("Goblin");
    CHECK(turns.next() == "Orc" && turns.next() == "Player", "circular remove");
}

static void testStackQueue() {
    section("Stack / Queue");
    Stack<int> st;
    st.push(1); st.push(2); st.push(3);
    CHECK(st.peek() == 3 && st.pop() == 3 && st.pop() == 2 && st.size() == 1, "stack LIFO");
    st.pop();
    CHECK(st.isEmpty(), "stack empty");
    bool threw = false;
    try { st.pop(); } catch (const std::out_of_range&) { threw = true; }
    CHECK(threw, "stack underflow throws");

    Queue<String> q;
    q.enqueue("attack"); q.enqueue("defend"); q.enqueue("heal");
    CHECK(q.front() == "attack" && q.dequeue() == "attack" && q.dequeue() == "defend", "queue FIFO");
    q.dequeue();
    CHECK(q.isEmpty(), "queue empty");
    q.enqueue("again");
    CHECK(q.front() == "again", "queue reusable after empty");
}

static void testHashTable() {
    section("Hash table");
    HashMode modes[3] = { CHAINING, LINEAR_PROBING, DOUBLE_HASHING };
    for (int m = 0; m < 3; m++) {
        HashTable inv(modes[m], 5);
        inv.addItem(Item("Sword", 10, 50));
        inv.addItem(Item("Potion", 1, 20, 2));
        inv.addItem(Item("potion", 1, 20, 3));          // same item, different case
        CHECK(inv.size() == 2 && inv.findItem("POTION")->quantity == 5, "add + merge quantity");
        for (int i = 0; i < 30; i++) inv.addItem(Item(String("gem") + String::fromInt(i), 1, i));
        CHECK(inv.size() == 32, "size after many adds");
        CHECK(inv.loadFactor() <= 0.7, "load factor kept under 0.7 by rehashing");
        CHECK(inv.getRehashCount() > 0, "rehash happened");
        CHECK(inv.removeItem("gem3") && !inv.findItem("gem3") && !inv.removeItem("gem3"), "remove");
        CHECK(inv.findItem("gem29") != nullptr, "find after remove (tombstone skipped)");
        inv.addItem(Item("gem3", 1, 3));
        CHECK(inv.findItem("gem3") != nullptr && inv.size() == 32, "re-add after remove");
        inv.removeItem("potion", 2);
        CHECK(inv.findItem("potion")->quantity == 3, "remove part of a stack");
        std::cout << HashTable::modeName(modes[m]) << ": capacity " << inv.getCapacity()
                  << ", rehashes " << inv.getRehashCount() << ", collisions " << inv.getCollisions() << "\n";
    }
    HashTable small(CHAINING);
    small.addItem(Item("Rusty Sword", 10, 5));
    small.addItem(Item("Health Potion", 1, 20, 3));
    small.listItems();
}

static void testStringMatching() {
    section("String matching");
    String text("abababcabababcab");
    String pat("ababc");
    MatchAlgorithm algos[4] = { BRUTE_FORCE, RABIN_KARP, BOYER_MOORE, KMP };
    for (int a = 0; a < 4; a++) {
        DynArray<int> pos;
        int found = getMatcher(algos[a])(text, pat, pos);
        CHECK(found == 2 && pos[0] == 2 && pos[1] == 9, matcherName(algos[a]));
        DynArray<int> overlap;
        CHECK(getMatcher(algos[a])("aaaaa", "aa", overlap) == 4, "overlapping matches");
        CHECK(getMatcher(algos[a])("abc", "xyz", overlap) == 0, "no match");
        CHECK(getMatcher(algos[a])("ab", "abc", overlap) == 0, "pattern longer than text");
    }
    std::cout << "KMP prefix table for \"ababaca\":\n";
    printPrefixTable("ababaca");
    int lps[7];
    buildPrefixTable("ababaca", lps);
    CHECK(lps[0] == 0 && lps[4] == 3 && lps[5] == 0 && lps[6] == 1, "KMP prefix table");
}

static void testParser() {
    section("Parser + game log");
    Command c = Parser::parse("  MOVE North ");
    CHECK(c.type == CMD_MOVE && c.valid && c.argument == "north", "move north");
    CHECK(Parser::parse("w").argument == "west", "shortcut w");
    c = Parser::parse("pick up Rusty Sword");
    CHECK(c.type == CMD_PICKUP && c.argument == "rusty sword", "pick up multi-word item");
    CHECK(Parser::parse("pickup key").argument == "key", "pickup one word");
    CHECK(Parser::parse("drop").valid == false, "drop needs an item");
    CHECK(Parser::parse("move up").valid == false, "bad direction");
    CHECK(Parser::parse("save").argument == "data/savegame.txt", "save default file");
    CHECK(Parser::parse("").type == CMD_EMPTY, "empty line");
    c = Parser::parse("search Goblin");
    CHECK(c.type == CMD_SEARCH && c.valid && c.argument == "goblin", "search command");
    CHECK(Parser::parse("search").valid == false, "search needs a word");
    CHECK(Parser::parse("quests").type == CMD_QUESTS && Parser::parse("catalog").type == CMD_CATALOG, "quests / catalog");
    c = Parser::parse("inventory weight");
    CHECK(c.type == CMD_INVENTORY && c.argument == "weight", "inventory weight");
    CHECK(Parser::parse("inventory colour").valid == false, "inventory bad sort key");
    c = Parser::parse("archive 2");
    CHECK(c.type == CMD_ARCHIVE && c.argument == "2", "archive lookup");
    CHECK(Parser::parse("serch goblin").message.find(String("search")) != -1, "suggest search");
    const char* simple[] = { "look", "undo", "redo", "hint", "inventory", "leaderboard", "attack", "load", "quit" };
    CommandType types[] = { CMD_LOOK, CMD_UNDO, CMD_REDO, CMD_HINT, CMD_INVENTORY, CMD_LEADERBOARD, CMD_ATTACK, CMD_LOAD, CMD_QUIT };
    for (int i = 0; i < 9; i++) CHECK(Parser::parse(simple[i]).type == types[i], simple[i]);
    c = Parser::parse("mvoe north");
    std::cout << "\"mvoe north\" -> " << c.message << "\n";
    CHECK(c.type == CMD_UNKNOWN && c.message.find(String("move")) != -1, "suggest move");
    c = Parser::parse("xyzzy");
    std::cout << "\"xyzzy\"      -> " << c.message << "\n";
    CHECK(Parser::editDistance("kitten", "sitting") == 3, "edit distance");

    GameLog log;
    log.add("You picked up a Rusty Sword.");
    log.add("A goblin attacks you!");
    log.add("You hit the Goblin for 5 damage.");
    log.add("You moved north.");
    std::cout << "Search log for 'goblin' (KMP):\n";
    for (int a = 0; a < 4; a++)
        CHECK(log.search("goblin", (MatchAlgorithm)a, a == KMP) == 2, "log search");
    CHECK(log.search("dragon", KMP, false) == 0, "log search miss");
}

static void testHistory() {
    section("Undo / redo");
    MoveHistory h;
    h.recordMove(Move(0, 0, 0, 1));
    h.recordMove(Move(0, 1, 1, 1));
    h.recordMove(Move(1, 1, 2, 1));
    h.printHistory();
    Move m;
    CHECK(h.undo(m) && m == Move(1, 1, 2, 1), "undo returns last move");
    CHECK(h.undo(m) && h.movesMade() == 1 && h.redoAvailable() == 2, "undo twice");
    CHECK(h.redo(m) && m == Move(0, 1, 1, 1), "redo");
    h.recordMove(Move(1, 1, 1, 2));
    CHECK(!h.canRedo(), "new move clears redo");
    h.undo(m); h.undo(m); h.undo(m);
    CHECK(!h.undo(m), "nothing left to undo");
}

static void testSaveLoad() {
    section("Save / load");
    const char* file = "test_save.txt";
    GameState gs;
    gs.playerName = "Mohsina";
    gs.playerRow = 4; gs.playerCol = 7; gs.hp = 80; gs.score = 150; gs.turn = 12; gs.seed = 2026;
    HashTable inv;
    inv.addItem(Item("Rusty Sword", 10, 5, 1, 101));
    inv.addItem(Item("Health Potion", 1, 20, 3, 102));
    String msg;
    CHECK(saveGame(file, gs, inv, msg), "save");
    std::cout << msg << "\n";

    GameState loaded;
    HashTable inv2;
    CHECK(loadGame(file, loaded, inv2, msg), "load");
    std::cout << msg << "\n";
    CHECK(loaded.playerName == "Mohsina" && loaded.playerRow == 4 && loaded.score == 150 && loaded.seed == 2026, "state restored");
    CHECK(inv2.size() == 2 && inv2.findItem("health potion")->quantity == 3 &&
          inv2.findItem("rusty sword")->id == 101, "inventory restored (with ids)");

    CHECK(!loadGame("no_such_file.txt", loaded, inv2, msg), "missing file");
    std::cout << msg << "\n";

    std::ofstream bad("test_bad.txt");
    bad << "DATAQUEST_SAVE 1\nNAME X\nSTATE 1 2 three 4 5 6\n";
    bad.close();
    CHECK(!loadGame("test_bad.txt", loaded, inv2, msg), "damaged file rejected");
    std::cout << msg << "\n";
    CHECK(loaded.playerName == "Mohsina" && inv2.size() == 2, "damaged file left game unchanged");
    std::remove(file);
    std::remove("test_bad.txt");
}

static void testRecursion() {
    section("Recursion");
    CHECK(factorial(0) == 1 && factorial(5) == 120 && factorial(20) == 2432902008176640000LL, "factorial");
    CHECK(fibonacci(0) == 0 && fibonacci(1) == 1 && fibonacci(10) == 55, "fibonacci");
    CHECK(fibonacciMemo(50) == 12586269025LL && fibonacciMemo(20) == fibonacci(20), "fibonacci memo");
    CHECK(towerOfHanoi(3, 'A', 'C', 'B', false) == 7 && towerOfHanoi(10, 'A', 'C', 'B', false) == 1023, "hanoi moves");
    runRecursionDemos();
}

static void benchmarkDemo() {
    section("Benchmarks (Module A)");
    runCoreBenchmarks();
}

int main() {
    std::cout << "===== DataQuest Module A tests =====\n";
    testString();
    testArrays();
    testLists();
    testStackQueue();
    testHashTable();
    testStringMatching();
    testParser();
    testHistory();
    testSaveLoad();
    testRecursion();
    benchmarkDemo();
    std::cout << "\n===== " << passed << " passed, " << failed << " failed =====\n";
    return failed == 0 ? 0 : 1;
}
