// SaveLoad.h - Module A (Mohsina Zeeshan)
// Saves / loads a game to a text file with fstream.
// A missing or damaged file is reported and the current game is NOT changed
// (everything is read into temporaries first, then copied over at the end).
//
// File format:
//   DATAQUEST_SAVE 1
//   NAME <player name>
//   STATE <row> <col> <hp> <score> <turn> <seed>
//   ITEMS <count>
//   <name>|<id>|<weight>|<value>|<quantity>      (one line per item)
//   END
//
// The dungeon is rebuilt from `seed`, so it does not need to be stored.
// Need more fields saved (monsters, quests, ...)? Ask Mohsina: GameState,
// saveGame and loadGame must all be updated together.
#ifndef DATAQUEST_SAVELOAD_H
#define DATAQUEST_SAVELOAD_H

#include "String.h"
#include "HashTable.h"

struct GameState {
    String playerName;
    int playerRow, playerCol;
    int hp;
    int score;
    int turn;
    int seed;          // random seed used to generate the dungeon

    GameState() : playerName("Player"), playerRow(0), playerCol(0), hp(100), score(0), turn(0), seed(0) {}
};

// Both return true on success. `message` always explains what happened.
bool saveGame(const String& filename, const GameState& state, const HashTable& inventory, String& message);
bool loadGame(const String& filename, GameState& state, HashTable& inventory, String& message);

#endif
