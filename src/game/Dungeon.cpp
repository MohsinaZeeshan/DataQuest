// Dungeon.cpp - Module C (Ayesha Nehal)
#include "../../include/game/Dungeon.h"
#include <cstdlib>
#include <cmath>

static int pseudoRand(int& seed, int minVal, int maxVal) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return minVal + (seed % (maxVal - minVal + 1));
}

Dungeon::Dungeon(int randomSeed)
    : grid(DUNGEON_HEIGHT, DUNGEON_WIDTH, TILE_WALL),
      roomItems(0),
      seed(randomSeed == 0 ? 1234567 : randomSeed),
      playerStartR(1), playerStartC(1),
      exitR(DUNGEON_HEIGHT - 2), exitC(DUNGEON_WIDTH - 2),
      exitRoomId(0) {
}

Dungeon::~Dungeon() {
}

char Dungeon::getTile(int r, int c) const {
    if (!grid.inBounds(r, c)) return TILE_WALL;
    return grid.at(r, c);
}

void Dungeon::setTile(int r, int c, char ch) {
    if (grid.inBounds(r, c)) {
        grid.at(r, c) = ch;
    }
}

bool Dungeon::isWalkable(int r, int c) const {
    if (!grid.inBounds(r, c)) return false;
    char ch = grid.at(r, c);
    return ch == TILE_FLOOR || ch == TILE_CORRIDOR || ch == TILE_EXIT;
}

int Dungeon::getRoomAt(int r, int c) const {
    for (int i = 0; i < rooms.size(); i++) {
        if (rooms[i].contains(r, c)) return i;
    }
    return -1;
}

void Dungeon::carveRoom(const Room& room) {
    for (int r = room.r; r < room.r + room.height; r++) {
        for (int c = room.c; c < room.c + room.width; c++) {
            if (grid.inBounds(r, c)) {
                grid.at(r, c) = TILE_FLOOR;
            }
        }
    }
}

// Recursive corridor carving with backtracking:
// Navigates from (curR, curC) to (targetR, targetC) inside bounds,
// carving walls into TILE_CORRIDOR and backtracking if trapped.
bool Dungeon::carveCorridorRecursive(int curR, int curC, int targetR, int targetC, Array2D<bool>& visited) {
    if (curR == targetR && curC == targetC) {
        if (grid.at(curR, curC) == TILE_WALL) grid.at(curR, curC) = TILE_CORRIDOR;
        return true;
    }

    visited.at(curR, curC) = true;
    if (grid.at(curR, curC) == TILE_WALL) {
        grid.at(curR, curC) = TILE_CORRIDOR;
    }

    // Direction priority: move closer to target first
    int dr = targetR - curR;
    int dc = targetC - curC;

    int rSteps[4];
    int cSteps[4];
    int idx = 0;

    // Pick primary axis
    if (std::abs(dr) >= std::abs(dc)) {
        if (dr != 0) { rSteps[idx] = (dr > 0 ? 1 : -1); cSteps[idx] = 0; idx++; }
        if (dc != 0) { rSteps[idx] = 0; cSteps[idx] = (dc > 0 ? 1 : -1); idx++; }
        if (dc != 0) { rSteps[idx] = 0; cSteps[idx] = (dc > 0 ? -1 : 1); idx++; }
        if (dr != 0) { rSteps[idx] = (dr > 0 ? -1 : 1); cSteps[idx] = 0; idx++; }
    } else {
        if (dc != 0) { rSteps[idx] = 0; cSteps[idx] = (dc > 0 ? 1 : -1); idx++; }
        if (dr != 0) { rSteps[idx] = (dr > 0 ? 1 : -1); cSteps[idx] = 0; idx++; }
        if (dr != 0) { rSteps[idx] = (dr > 0 ? -1 : 1); cSteps[idx] = 0; idx++; }
        if (dc != 0) { rSteps[idx] = 0; cSteps[idx] = (dc > 0 ? -1 : 1); idx++; }
    }

    for (int i = 0; i < idx; i++) {
        int nextR = curR + rSteps[i];
        int nextC = curC + cSteps[i];
        if (nextR > 0 && nextR < DUNGEON_HEIGHT - 1 &&
            nextC > 0 && nextC < DUNGEON_WIDTH - 1 &&
            !visited.at(nextR, nextC)) {
            if (carveCorridorRecursive(nextR, nextC, targetR, targetC, visited)) {
                return true;
            }
        }
    }

    return false;
}

void Dungeon::generate(int targetRooms, int randomSeed) {
    if (randomSeed != -1) seed = randomSeed;
    if (targetRooms < 8) targetRooms = 8;
    if (targetRooms > 12) targetRooms = 12;

    grid.fill(TILE_WALL);
    rooms.clear();
    monsters.clear();

    // 1. Place 8 to 12 non-overlapping rooms inside the 20x20 grid
    int attempts = 0;
    while (rooms.size() < targetRooms && attempts < 500) {
        attempts++;
        int w = pseudoRand(seed, 3, 4);
        int h = pseudoRand(seed, 3, 4);
        int r = pseudoRand(seed, 1, DUNGEON_HEIGHT - h - 2);
        int c = pseudoRand(seed, 1, DUNGEON_WIDTH - w - 2);

        Room candidate(rooms.size(), r, c, h, w);
        bool overlaps = false;
        for (int i = 0; i < rooms.size(); i++) {
            if (candidate.intersects(rooms[i], 1)) {
                overlaps = true;
                break;
            }
        }

        if (!overlaps) {
            rooms.pushBack(candidate);
            carveRoom(candidate);
        }
    }

    // If tight grid prevented targetRooms, guarantee at least 8 rooms with compact sizes
    while (rooms.size() < 8) {
        int r = 1 + (rooms.size() / 3) * 6;
        int c = 1 + (rooms.size() % 3) * 6;
        Room fallback(rooms.size(), r, c, 3, 3);
        rooms.pushBack(fallback);
        carveRoom(fallback);
    }

    // 2. Connect consecutive rooms using recursive backtracking corridors
    for (int i = 0; i < rooms.size() - 1; i++) {
        Array2D<bool> visited(DUNGEON_HEIGHT, DUNGEON_WIDTH, false);
        carveCorridorRecursive(rooms[i].centerR, rooms[i].centerC,
                               rooms[i + 1].centerR, rooms[i + 1].centerC, visited);
    }
    // Also connect last room to first to create alternative pathways
    {
        Array2D<bool> visited(DUNGEON_HEIGHT, DUNGEON_WIDTH, false);
        carveCorridorRecursive(rooms[rooms.size() - 1].centerR, rooms[rooms.size() - 1].centerC,
                               rooms[0].centerR, rooms[0].centerC, visited);
    }

    // 3. Player start in Room 0; Exit in furthest room
    playerStartR = rooms[0].centerR;
    playerStartC = rooms[0].centerC;

    exitRoomId = rooms.size() - 1;
    exitR = rooms[exitRoomId].centerR;
    exitC = rooms[exitRoomId].centerC;
    grid.at(exitR, exitC) = TILE_EXIT;

    // 4. Per-room item lists using JaggedArray
    roomItems.reset(rooms.size());
    const char* sampleItems[] = { "Health Potion", "Iron Sword", "Magic Scroll", "Shield", "Torch", "Golden Key", "Mana Elixir" };
    int sampleWeights[] = { 1, 5, 1, 4, 2, 1, 1 };
    int sampleValues[]  = { 20, 50, 40, 35, 10, 100, 30 };

    for (int i = 0; i < rooms.size(); i++) {
        if (i == 0) continue; // no items in player spawn room
        int itemIdx = pseudoRand(seed, 0, 6);
        Item it(sampleItems[itemIdx], sampleWeights[itemIdx], sampleValues[itemIdx], 1);
        roomItems.addToRow(i, it);

        // Put golden key in room before exit
        if (i == exitRoomId - 1) {
            Item key("Dungeon Key", 1, 150, 1);
            roomItems.addToRow(i, key);
        }
    }

    // 5. Place Monsters in intermediate rooms
    const char* monsterNames[] = { "Goblin", "Skeleton", "Orc Warrior", "Cave Spider", "Dungeon Golem" };
    // Dungeon Golem guards the exit. At 80 HP / 18 attack it needed 7 player hits
    // and dealt 108 damage first, so the game could not be won without a lucky
    // sword. At 60 HP / 14 attack it costs about 56 HP: beatable after healing.
    int mHp[]    = { 25, 35, 50, 20, 60 };
    int mAtk[]   = { 6, 9, 14, 8, 14 };
    int mSpeed[] = { 12, 10, 8, 15, 5 };

    int mId = 1;
    for (int i = 1; i < rooms.size(); i++) {
        if (i % 2 == 1 || i == exitRoomId) {
            int kind = (i == exitRoomId) ? 4 : pseudoRand(seed, 0, 3);
            Monster m(mId++, monsterNames[kind], rooms[i].centerR, rooms[i].centerC,
                      mHp[kind], mAtk[kind], mSpeed[kind], i);
            monsters.pushBack(m);
        }
    }
}

bool Dungeon::takeItemFromRoom(int roomId, const String& itemName, Item& outItem) {
    if (roomId < 0 || roomId >= roomItems.numRows()) return false;
    for (int i = 0; i < roomItems.rowSize(roomId); i++) {
        if (roomItems.get(roomId, i).name.compare(itemName) == 0 ||
            roomItems.get(roomId, i).name.toLower() == itemName.toLower()) {
            outItem = roomItems.get(roomId, i);
            roomItems.removeFromRow(roomId, i);
            return true;
        }
    }
    return false;
}

void Dungeon::addItemToRoom(int roomId, const Item& item) {
    if (roomId >= 0 && roomId < roomItems.numRows()) {
        roomItems.addToRow(roomId, item);
    }
}

Monster* Dungeon::getMonsterAt(int r, int c) {
    for (int i = 0; i < monsters.size(); i++) {
        if (monsters[i].alive && monsters[i].r == r && monsters[i].c == c) {
            return &monsters[i];
        }
    }
    return nullptr;
}

void Dungeon::removeDeadMonsters() {
    for (int i = monsters.size() - 1; i >= 0; i--) {
        if (!monsters[i].alive) {
            monsters.removeAt(i);
        }
    }
}

void Dungeon::printMap(int playerR, int playerC, std::ostream& os) const {
    os << "\n";
    os << "     ";
    for (int c = 0; c < DUNGEON_WIDTH; c++) {
        os << (c % 10) << " ";
    }
    os << "\n";
    os << "    +";
    for (int c = 0; c < DUNGEON_WIDTH; c++) {
        os << "--";
    }
    os << "+\n";

    for (int r = 0; r < DUNGEON_HEIGHT; r++) {
        os << (r < 10 ? " " : "") << r << " | ";
        for (int c = 0; c < DUNGEON_WIDTH; c++) {
            char tileChar = grid.at(r, c);

            if (r == playerR && c == playerC) {
                tileChar = TILE_PLAYER;
            } else {
                // Check monster
                bool drewMonster = false;
                for (int m = 0; m < monsters.size(); m++) {
                    if (monsters[m].alive && monsters[m].r == r && monsters[m].c == c) {
                        tileChar = TILE_MONSTER;
                        drewMonster = true;
                        break;
                    }
                }
                if (!drewMonster) {
                    // Check exit
                    if (r == exitR && c == exitC) {
                        tileChar = TILE_EXIT;
                    } else {
                        // Check items in room
                        int rmId = getRoomAt(r, c);
                        if (rmId >= 0 && rmId < roomItems.numRows() && roomItems.rowSize(rmId) > 0) {
                            if (r == rooms[rmId].centerR && c == rooms[rmId].centerC) {
                                tileChar = TILE_ITEM;
                            }
                        }
                    }
                }
            }
            os << tileChar << " ";
        }
        os << "|\n";
    }

    os << "    +";
    for (int c = 0; c < DUNGEON_WIDTH; c++) {
        os << "--";
    }
    os << "+\n";

    os << "  MAP LEGEND:\n";
    os << "    [@] Player (You)       [M] Monster (Danger!)   [*] Item (Pick Up)\n";
    os << "    [E] Dungeon Exit       [.] Room Floor          [+] Corridor / Hallway\n";
    os << "    [#] Solid Wall (Impassable)\n";
}
