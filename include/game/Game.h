// Game.h - Module C (Ayesha Nehal)
// Game engine and loop:
// - ASCII rendering and world updates
// - Turn-based combat using:
//     * CircularList for turn cycles
//     * MaxHeap for combat turn order (who acts first)
//     * Queue for action queue
// - Connects inventory, item catalog, quests, undo/redo, hints, leaderboard, save/load
// - Win and lose conditions with AVL leaderboard integration
#ifndef DATAQUEST_GAME_H
#define DATAQUEST_GAME_H

#include "../core/String.h"
#include "../core/DynArray.h"
#include "../core/LinkedList.h"
#include "../core/Queue.h"
#include "../core/HashTable.h"
#include "../core/History.h"
#include "../core/Parser.h"
#include "../core/SaveLoad.h"
#include "../tree/Heap.h"
#include "../tree/BST.h"
#include "../tree/AVL.h"
#include "Dungeon.h"
#include "Graph.h"
#include "HintSolver.h"
#include <iostream>

struct Combatant {
    String name;
    int hp;
    int maxHp;
    int attack;
    int speed;
    bool isPlayer;

    Combatant() : hp(0), maxHp(0), attack(0), speed(0), isPlayer(false) {}
    Combatant(const String& n, int h, int atk, int spd, bool p)
        : name(n), hp(h), maxHp(h), attack(atk), speed(spd), isPlayer(p) {}

    bool operator==(const Combatant& o) const {
        return name == o.name && isPlayer == o.isPlayer;
    }
};

enum ActionType { ACT_ATTACK, ACT_HEAL, ACT_FLEE };

struct CombatAction {
    ActionType type;
    String actorName;
    CombatAction() : type(ACT_ATTACK) {}
    CombatAction(ActionType t, const String& actor) : type(t), actorName(actor) {}
};

class Game {
private:
    Dungeon dungeon;
    RoomGraph roomGraph;
    QuestManager questManager;
    BST itemCatalog;
    AVLTree leaderboard;
    HashTable inventory;
    MoveHistory moveHistory;
    GameLog gameLog;

    String playerName;
    int playerR, playerC;
    int playerHp;
    int playerMaxHp;
    int playerAttack;
    int playerSpeed;
    int score;
    int turnCount;
    bool gameOver;
    bool gameWon;
    int seed;

    void setupRoomGraph();
    void setupQuests();
    void setupCatalog();
    void setupLeaderboard();

    // Combat engine using studied structures: CircularList, MaxHeap, Queue
    bool resolveCombat(Monster& monster);

public:
    explicit Game(int gameSeed = 12345);
    ~Game();

    void initializeGame(const String& name = "Adventurer", int seed = 12345);

    // Main game loop
    void run();

    // Command handlers
    void handleMove(const String& dir);
    void handleLook();
    void handlePickUp(const String& itemName);
    void handleDrop(const String& itemName);
    void handleUse(const String& itemName);
    void handleAttack();
    void handleUndo();
    void handleRedo();
    void handleHint();
    void handleInventory();
    void handleLeaderboard();
    void handleSave(const String& filename);
    void handleLoad(const String& filename);
    void handleCatalog();
    void handleQuests();
    void handleBenchmark();

    // Status checks
    bool isGameOver() const { return gameOver; }
    bool isGameWon() const { return gameWon; }
    int getScore() const { return score; }
    int getPlayerHp() const { return playerHp; }
    int getPlayerR() const { return playerR; }
    int getPlayerC() const { return playerC; }
    const Dungeon& getDungeon() const { return dungeon; }
    Dungeon& getDungeon() { return dungeon; }
    const RoomGraph& getRoomGraph() const { return roomGraph; }
    const QuestManager& getQuestManager() const { return questManager; }
    const BST& getItemCatalog() const { return itemCatalog; }
    const AVLTree& getLeaderboard() const { return leaderboard; }
    const HashTable& getInventory() const { return inventory; }
    const MoveHistory& getMoveHistory() const { return moveHistory; }
};

#endif
