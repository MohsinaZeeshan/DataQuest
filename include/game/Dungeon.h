// Dungeon.h - Module C (Ayesha Nehal)
// Procedural 20x20 dungeon generator:
// - 20x20 grid using Array2D<char>
// - 8 to 12 randomly placed non-overlapping rooms
// - Recursive backtracking corridor carver
// - Per-room item lists using JaggedArray<Item>
// - Placement of player start, exit, monsters, and items
#ifndef DATAQUEST_DUNGEON_H
#define DATAQUEST_DUNGEON_H

#include "../core/DynArray.h"
#include "../core/String.h"
#include "../core/HashTable.h"

// Map tile symbols
const char TILE_WALL     = '#';
const char TILE_FLOOR    = '.';
const char TILE_CORRIDOR = '+';
const char TILE_PLAYER   = '@';
const char TILE_MONSTER  = 'M';
const char TILE_ITEM     = '*';
const char TILE_EXIT     = 'E';

const int DUNGEON_WIDTH  = 20;
const int DUNGEON_HEIGHT = 20;

struct Room {
    int id;
    int r, c;            // top-left coordinates
    int height, width;
    int centerR, centerC;

    Room() : id(0), r(0), c(0), height(0), width(0), centerR(0), centerC(0) {}
    Room(int id_, int r_, int c_, int h_, int w_)
        : id(id_), r(r_), c(c_), height(h_), width(w_),
          centerR(r_ + h_ / 2), centerC(c_ + w_ / 2) {}

    bool intersects(const Room& other, int padding = 1) const {
        return !(r + height + padding <= other.r ||
                 other.r + other.height + padding <= r ||
                 c + width + padding <= other.c ||
                 other.c + other.width + padding <= c);
    }

    bool contains(int row, int col) const {
        return row >= r && row < r + height && col >= c && col < c + width;
    }
};

struct Monster {
    int id;
    String name;
    int r, c;
    int hp;
    int maxHp;
    int attack;
    int speed;      // Priority in combat max-heap
    bool alive;
    int roomId;

    Monster() : id(0), r(0), c(0), hp(0), maxHp(0), attack(0), speed(0), alive(false), roomId(0) {}
    Monster(int id_, const String& name_, int r_, int c_, int hp_, int atk_, int spd_, int rm_)
        : id(id_), name(name_), r(r_), c(c_), hp(hp_), maxHp(hp_), attack(atk_), speed(spd_), alive(true), roomId(rm_) {}
};

class Dungeon {
private:
    Array2D<char> grid;            // 20x20 base terrain
    DynArray<Room> rooms;          // 8 to 12 placed rooms
    JaggedArray<Item> roomItems;   // per-room item lists
    DynArray<Monster> monsters;    // spawned monsters
    int seed;

    int playerStartR, playerStartC;
    int exitR, exitC;
    int exitRoomId;

    // Recursive corridor carving helper with backtracking
    bool carveCorridorRecursive(int curR, int curC, int targetR, int targetC, Array2D<bool>& visited);

    // Carves a rectangular room on the grid
    void carveRoom(const Room& room);

public:
    Dungeon(int randomSeed = 0);
    ~Dungeon();

    // Generates a complete 20x20 dungeon
    void generate(int targetRooms = 9, int randomSeed = -1);

    // Grid inspection
    int getRows() const { return DUNGEON_HEIGHT; }
    int getCols() const { return DUNGEON_WIDTH; }
    char getTile(int r, int c) const;
    void setTile(int r, int c, char ch);
    bool isWalkable(int r, int c) const;
    bool inBounds(int r, int c) const { return grid.inBounds(r, c); }

    // Rooms & Items
    int getRoomCount() const { return rooms.size(); }
    const Room& getRoom(int idx) const { return rooms[idx]; }
    const DynArray<Room>& getRooms() const { return rooms; }
    int getRoomAt(int r, int c) const;

    JaggedArray<Item>& getRoomItems() { return roomItems; }
    const JaggedArray<Item>& getRoomItems() const { return roomItems; }
    bool takeItemFromRoom(int roomId, const String& itemName, Item& outItem);
    void addItemToRoom(int roomId, const Item& item);

    // Monsters
    int getMonsterCount() const { return monsters.size(); }
    Monster* getMonster(int idx) { return &monsters[idx]; }
    const Monster* getMonster(int idx) const { return &monsters[idx]; }
    Monster* getMonsterAt(int r, int c);
    void removeDeadMonsters();

    // Player and Exit
    int getPlayerStartR() const { return playerStartR; }
    int getPlayerStartC() const { return playerStartC; }
    int getExitR() const { return exitR; }
    int getExitC() const { return exitC; }
    int getExitRoomId() const { return exitRoomId; }
    int getSeed() const { return seed; }

    // Renders the ASCII map with overlay (player, monsters, items, exit)
    void printMap(int playerR, int playerC, std::ostream& os = std::cout) const;
};

#endif
