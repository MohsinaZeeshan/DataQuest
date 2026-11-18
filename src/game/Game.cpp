// Game.cpp - Module C (Ayesha Nehal)
#include "../../include/game/Game.h"
#include <iostream>
#include <cmath>

Game::Game(int gameSeed)
    : dungeon(gameSeed),
      playerName("Ayesha"),
      playerR(1), playerC(1),
      playerHp(100), playerMaxHp(100),
      playerAttack(12), playerSpeed(10),
      score(0), turnCount(0),
      gameOver(false), gameWon(false),
      seed(gameSeed) {
    initializeGame(playerName, gameSeed);
}

Game::~Game() {
}

void Game::initializeGame(const String& name, int s) {
    playerName = name;
    seed = s;
    turnCount = 0;
    score = 0;
    gameOver = false;
    gameWon = false;
    playerHp = 100;
    playerMaxHp = 100;
    playerAttack = 12;
    playerSpeed = 10;

    dungeon.generate(9, seed);
    playerR = dungeon.getPlayerStartR();
    playerC = dungeon.getPlayerStartC();

    setupRoomGraph();
    setupQuests();
    setupCatalog();
    setupLeaderboard();

    inventory.clear();
    Item starterPotion("Health Potion", 1, 20, 2);
    inventory.addItem(starterPotion);

    moveHistory.clear();
    gameLog.clear();
    gameLog.add("Welcome to DataQuest! Explore the dungeon, collect items, fight monsters and find the exit.");
}

void Game::setupRoomGraph() {
    int roomCount = dungeon.getRoomCount();
    roomGraph.init(roomCount);

    // Compute pairwise distances between all room centers
    DynArray<Edge> allEdges;
    const DynArray<Room>& rooms = dungeon.getRooms();
    for (int i = 0; i < roomCount; i++) {
        for (int j = i + 1; j < roomCount; j++) {
            int dr = rooms[i].centerR - rooms[j].centerR;
            int dc = rooms[i].centerC - rooms[j].centerC;
            int dist = (int)std::sqrt(dr * dr + dc * dc);
            allEdges.pushBack(Edge(i, j, dist));
        }
    }

    // Kruskal's Minimum Spanning Tree to pick core corridor connections
    DynArray<Edge> mst = RoomGraph::kruskalMST(roomCount, allEdges);
    for (int i = 0; i < mst.size(); i++) {
        roomGraph.addEdge(mst[i].u, mst[i].v, mst[i].weight, true);
    }

    // Also connect adjacent sequence to allow cycles in dungeon layout
    for (int i = 0; i < roomCount - 1; i++) {
        if (!roomGraph.hasEdge(i, i + 1)) {
            int dr = rooms[i].centerR - rooms[i + 1].centerR;
            int dc = rooms[i].centerC - rooms[i + 1].centerC;
            int dist = (int)std::sqrt(dr * dr + dc * dc);
            roomGraph.addEdge(i, i + 1, dist, true);
        }
    }
}

void Game::setupQuests() {
    questManager.addQuest(0, "Awakening", "Explore your starting chamber.", 50);
    questManager.addQuest(1, "Combat Trial", "Defeat a dungeon monster in combat.", 100);
    questManager.addQuest(2, "Keymaster", "Find the Dungeon Key hidden in the chambers.", 150);
    questManager.addQuest(3, "Escape", "Unlock and enter the Dungeon Exit.", 200);

    questManager.addDependency(0, 1);
    questManager.addDependency(1, 2);
    questManager.addDependency(2, 3);
}

void Game::setupCatalog() {
    itemCatalog.insert(CatalogItem(1, "Health Potion", 1, 20));
    itemCatalog.insert(CatalogItem(2, "Iron Sword", 5, 50));
    itemCatalog.insert(CatalogItem(3, "Magic Scroll", 1, 40));
    itemCatalog.insert(CatalogItem(4, "Shield", 4, 35));
    itemCatalog.insert(CatalogItem(5, "Torch", 2, 10));
    itemCatalog.insert(CatalogItem(6, "Dungeon Key", 1, 150));
    itemCatalog.insert(CatalogItem(7, "Mana Elixir", 1, 30));
}

void Game::setupLeaderboard() {
    leaderboard.insert("Arthur", 450);
    leaderboard.insert("Galahad", 380);
    leaderboard.insert("Lancelot", 520);
    leaderboard.insert("Merlin", 600);
    leaderboard.insert("Robin", 210);
}

// Combat system using studied structures:
// - MaxHeap: determines initiative / who acts first based on speed
// - CircularList: cycles turns between combatants
// - Queue: processes combat action queue
bool Game::resolveCombat(Monster& monster) {
    std::cout << "\n============================================\n";
    std::cout << " BATTLE ENGAGED: " << playerName << " vs " << monster.name << "!\n";
    std::cout << "============================================\n";

    Combatant p(playerName, playerHp, playerAttack, playerSpeed, true);
    Combatant m(monster.name, monster.hp, monster.attack, monster.speed, false);

    // 1. Max-Heap for Initiative (who acts first)
    MaxHeap<Combatant> initiativeHeap(4);
    initiativeHeap.insert(p, p.speed);
    initiativeHeap.insert(m, m.speed);

    Combatant first = initiativeHeap.extractMax();
    Combatant second = initiativeHeap.extractMax();

    std::cout << "Initiative Order: " << first.name << " acts first (Speed "
              << first.speed << ") followed by " << second.name << " (Speed "
              << second.speed << ").\n\n";

    // 2. Circular Linked List for turn cycle
    CircularList<Combatant> turnCycle;
    turnCycle.insert(first);
    turnCycle.insert(second);

    // 3. Queue for Action Queue
    Queue<CombatAction> actionQueue;

    int round = 1;
    while (p.hp > 0 && m.hp > 0 && round <= 30) {
        std::cout << "--- Round " << round++ << " ---\n";

        Combatant& currentActor = turnCycle.current();

        // Enqueue action into action queue
        if (currentActor.isPlayer) {
            actionQueue.enqueue(CombatAction(ACT_ATTACK, playerName));
        } else {
            actionQueue.enqueue(CombatAction(ACT_ATTACK, monster.name));
        }

        // Dequeue and execute action
        while (!actionQueue.isEmpty()) {
            CombatAction act = actionQueue.dequeue();
            if (act.actorName == playerName) {
                int dmg = p.attack;
                m.hp -= dmg;
                std::cout << ">> " << playerName << " strikes " << monster.name
                          << " for " << dmg << " damage! (" << monster.name << " HP: "
                          << (m.hp > 0 ? m.hp : 0) << "/" << monster.maxHp << ")\n";
            } else {
                int dmg = m.attack;
                p.hp -= dmg;
                std::cout << ">> " << monster.name << " attacks " << playerName
                          << " for " << dmg << " damage! (" << playerName << " HP: "
                          << (p.hp > 0 ? p.hp : 0) << "/" << playerMaxHp << ")\n";
            }
        }

        if (m.hp <= 0) break;
        if (p.hp <= 0) break;

        // Advance to next turn in circular list
        turnCycle.next();
    }

    playerHp = p.hp;
    if (playerHp <= 0) {
        playerHp = 0;
        gameOver = true;
        gameWon = false;
        std::cout << "\nYou were defeated by " << monster.name << " in battle...\n";
        leaderboard.insert(playerName, score);
        return false;
    }

    monster.alive = false;
    monster.hp = 0;
    score += 75;
    std::cout << "\nVictory! You defeated " << monster.name << "! (+75 points)\n";
    dungeon.removeDeadMonsters();
    return true;
}

void Game::handleMove(const String& dir) {
    int dr = 0, dc = 0;
    String d = dir.toLower();
    if (d == "north" || d == "n" || d == "up")    dr = -1;
    else if (d == "south" || d == "s" || d == "down")  dr = 1;
    else if (d == "west"  || d == "w" || d == "left")  dc = -1;
    else if (d == "east"  || d == "e" || d == "right") dc = 1;
    else {
        std::cout << "Unknown direction '" << dir << "'. Use north, south, east, or west.\n";
        return;
    }

    int nr = playerR + dr;
    int nc = playerC + dc;

    if (!dungeon.inBounds(nr, nc) || !dungeon.isWalkable(nr, nc)) {
        std::cout << "A solid stone wall blocks your path.\n";
        return;
    }

    // Check if monster on destination tile
    Monster* mon = dungeon.getMonsterAt(nr, nc);
    if (mon && mon->alive) {
        std::cout << "A fierce " << mon->name << " stands before you!\n";
        if (!resolveCombat(*mon)) {
            return;
        }
    }

    // Record move in MoveHistory for undo/redo
    moveHistory.recordMove(Move(playerR, playerC, nr, nc));
    playerR = nr;
    playerC = nc;
    turnCount++;

    // Check if player entered a room
    int roomId = dungeon.getRoomAt(playerR, playerC);
    if (roomId != -1) {
        JaggedArray<Item>& roomItems = dungeon.getRoomItems();
        if (roomItems.rowSize(roomId) > 0) {
            std::cout << "You see items lying in this chamber: ";
            for (int i = 0; i < roomItems.rowSize(roomId); i++) {
                std::cout << roomItems.get(roomId, i).name;
                if (i + 1 < roomItems.rowSize(roomId)) std::cout << ", ";
            }
            std::cout << "\n";
        }
    }

    // Check exit
    if (playerR == dungeon.getExitR() && playerC == dungeon.getExitC()) {
        const Item* key = inventory.findItem("Dungeon Key");
        if (key != nullptr || roomId == dungeon.getExitRoomId()) {
            gameWon = true;
            gameOver = true;
            score += 200 + playerHp * 2;
            std::cout << "\n============================================\n";
            std::cout << " CONGRATULATIONS! YOU HAVE ESCAPED THE DUNGEON!\n";
            std::cout << " Final Score: " << score << " points in " << turnCount << " turns!\n";
            std::cout << "============================================\n";
            leaderboard.insert(playerName, score);
            return;
        } else {
            std::cout << "The Dungeon Exit is sealed shut! You need the 'Dungeon Key' to escape.\n";
        }
    }

    // Monster pathfinding chase check using room graph BFS
    int currentRoom = dungeon.getRoomAt(playerR, playerC);
    if (currentRoom != -1) {
        DynArray<int> distances;
        roomGraph.bfs(currentRoom, distances);
        for (int m = 0; m < dungeon.getMonsterCount(); m++) {
            Monster* monster = dungeon.getMonster(m);
            if (monster && monster->alive) {
                int monRoom = dungeon.getRoomAt(monster->r, monster->c);
                if (monRoom != -1 && distances[monRoom] == 1) {
                    std::cout << "You hear the ominous footsteps of a " << monster->name
                              << " in an adjacent chamber!\n";
                }
            }
        }
    }
}

void Game::handleLook() {
    int roomId = dungeon.getRoomAt(playerR, playerC);
    if (roomId != -1) {
        std::cout << "You are standing inside Chamber #" << roomId << ".\n";
        const JaggedArray<Item>& rItems = dungeon.getRoomItems();
        if (roomId < rItems.numRows() && rItems.rowSize(roomId) > 0) {
            std::cout << "Items in this room:\n";
            for (int i = 0; i < rItems.rowSize(roomId); i++) {
                const Item& it = rItems.get(roomId, i);
                std::cout << "  - " << it.name << " (weight: " << it.weight
                          << ", value: " << it.value << ")\n";
            }
        } else {
            std::cout << "There are no items left in this room.\n";
        }
    } else {
        std::cout << "You are traversing a dimly lit dungeon corridor.\n";
    }

    // Check adjacent tiles for monsters
    for (int m = 0; m < dungeon.getMonsterCount(); m++) {
        const Monster* monster = dungeon.getMonster(m);
        if (monster && monster->alive) {
            int dist = std::abs(monster->r - playerR) + std::abs(monster->c - playerC);
            if (dist <= 2) {
                std::cout << "Nearby: " << monster->name << " at (" << monster->r
                          << ", " << monster->c << ") [HP: " << monster->hp << "].\n";
            }
        }
    }
}

void Game::handlePickUp(const String& itemName) {
    int roomId = dungeon.getRoomAt(playerR, playerC);
    if (roomId == -1) {
        std::cout << "You cannot pick up items in corridors.\n";
        return;
    }

    Item pickedItem;
    if (dungeon.takeItemFromRoom(roomId, itemName, pickedItem)) {
        inventory.addItem(pickedItem);
        score += pickedItem.value / 2;
        std::cout << "Picked up " << pickedItem.name << " and stashed it in inventory.\n";
    } else {
        std::cout << "No item named '" << itemName << "' found in this room.\n";
    }
}

void Game::handleDrop(const String& itemName) {
    int roomId = dungeon.getRoomAt(playerR, playerC);
    if (roomId == -1) {
        std::cout << "You can only drop items inside rooms.\n";
        return;
    }

    Item* it = inventory.findItem(itemName);
    if (!it) {
        std::cout << "You do not possess '" << itemName << "'.\n";
        return;
    }

    Item dropped(it->name, it->weight, it->value, 1);
    inventory.removeItem(itemName, 1);
    dungeon.addItemToRoom(roomId, dropped);
    std::cout << "Dropped " << dropped.name << " on the room floor.\n";
}

void Game::handleUse(const String& itemName) {
    Item* it = inventory.findItem(itemName);
    if (!it) {
        std::cout << "You don't have '" << itemName << "' in your inventory.\n";
        return;
    }

    String lowerName = it->name.toLower();
    if (lowerName.find("potion") != -1 || lowerName.find("elixir") != -1) {
        int healAmt = 35;
        playerHp += healAmt;
        if (playerHp > playerMaxHp) playerHp = playerMaxHp;
        inventory.removeItem(it->name, 1);
        std::cout << "You consumed " << it->name << " and restored " << healAmt
                  << " HP! (Current HP: " << playerHp << "/" << playerMaxHp << ")\n";
    } else if (lowerName.find("sword") != -1) {
        playerAttack += 5;
        std::cout << "You brandished " << it->name << "! Attack power increased to "
                  << playerAttack << ".\n";
    } else {
        std::cout << "You cannot directly use " << it->name << " right now.\n";
    }
}

void Game::handleAttack() {
    // Attack adjacent monster if any
    for (int m = 0; m < dungeon.getMonsterCount(); m++) {
        Monster* mon = dungeon.getMonster(m);
        if (mon && mon->alive) {
            int dist = std::abs(mon->r - playerR) + std::abs(mon->c - playerC);
            if (dist <= 1) {
                resolveCombat(*mon);
                return;
            }
        }
    }
    std::cout << "No monsters within melee striking distance.\n";
}

void Game::handleUndo() {
    Move m;
    if (moveHistory.undo(m)) {
        playerR = m.fromRow;
        playerC = m.fromCol;
        std::cout << "Undid move. Player returned to (" << playerR << ", " << playerC << ").\n";
    } else {
        std::cout << "No moves available to undo.\n";
    }
}

void Game::handleRedo() {
    Move m;
    if (moveHistory.redo(m)) {
        playerR = m.toRow;
        playerC = m.toCol;
        std::cout << "Redid move. Player moved to (" << playerR << ", " << playerC << ").\n";
    } else {
        std::cout << "No moves available to redo.\n";
    }
}

void Game::handleHint() {
    String hint = HintSolver::getNextStepHint(dungeon, playerR, playerC, true);
    std::cout << hint << "\n";
}

void Game::handleInventory() {
    inventory.listItems();
}

void Game::handleLeaderboard() {
    leaderboard.printTopN(10);
}

void Game::handleSave(const String& filename) {
    GameState st;
    st.playerName = playerName;
    st.playerRow = playerR;
    st.playerCol = playerC;
    st.hp = playerHp;
    st.score = score;
    st.turn = turnCount;
    st.seed = seed;

    String msg;
    String fn = filename.empty() ? String("savegame.txt") : filename;
    if (saveGame(fn, st, inventory, msg)) {
        std::cout << "Game successfully saved to " << fn << ".\n";
    } else {
        std::cout << "Save error: " << msg << "\n";
    }
}

void Game::handleLoad(const String& filename) {
    GameState st;
    String msg;
    String fn = filename.empty() ? String("savegame.txt") : filename;
    if (loadGame(fn, st, inventory, msg)) {
        playerName = st.playerName;
        playerR = st.playerRow;
        playerC = st.playerCol;
        playerHp = st.hp;
        score = st.score;
        turnCount = st.turn;
        seed = st.seed;
        dungeon.generate(9, seed);
        setupRoomGraph();
        std::cout << "Game successfully loaded from " << fn << ".\n";
    } else {
        std::cout << "Load error: " << msg << "\n";
    }
}

void Game::handleCatalog() {
    itemCatalog.printSorted();
}

void Game::handleQuests() {
    std::cout << "=== Dungeon Quests (Topological Dependency Order) ===\n";
    DynArray<Quest> order;
    String errMsg;
    if (questManager.topologicalOrder(order, errMsg)) {
        for (int i = 0; i < order.size(); i++) {
            std::cout << "  Step " << (i + 1) << ": [" << order[i].name << "] - "
                      << order[i].description << " (" << order[i].rewardExp << " XP)\n";
        }
    } else {
        std::cout << "Quest error: " << errMsg << "\n";
    }
}

void Game::handleBenchmark() {
    std::cout << "\n=== Benchmark and Demonstration Mode ===\n";
    HintSolver::runDemos(std::cout);
}

void Game::run() {
    std::cout << "\n============================================\n";
    std::cout << "       WELCOME TO DATAQUEST: DUNGEON CRAWLER\n";
    std::cout << "============================================\n";
    std::cout << "Type 'help' for a full list of commands.\n\n";

    while (!gameOver) {
        dungeon.printMap(playerR, playerC);
        std::cout << "\n[" << playerName << " | HP: " << playerHp << "/" << playerMaxHp
                  << " | Score: " << score << " | Pos: (" << playerR << "," << playerC << ")]\n";
        std::cout << "Command > ";

        String line;
        if (!String::readLine(std::cin, line)) break;
        if (line.empty()) continue;

        Command cmd = Parser::parse(line);
        if (!cmd.valid) {
            std::cout << cmd.message << "\n";
            continue;
        }

        switch (cmd.type) {
            case CMD_MOVE:
                handleMove(cmd.argument);
                break;
            case CMD_LOOK:
                handleLook();
                break;
            case CMD_PICKUP:
                handlePickUp(cmd.argument);
                break;
            case CMD_DROP:
                handleDrop(cmd.argument);
                break;
            case CMD_USE:
                handleUse(cmd.argument);
                break;
            case CMD_ATTACK:
                handleAttack();
                break;
            case CMD_UNDO:
                handleUndo();
                break;
            case CMD_REDO:
                handleRedo();
                break;
            case CMD_HINT:
                handleHint();
                break;
            case CMD_INVENTORY:
                handleInventory();
                break;
            case CMD_LEADERBOARD:
                handleLeaderboard();
                break;
            case CMD_SAVE:
                handleSave(cmd.argument);
                break;
            case CMD_LOAD:
                handleLoad(cmd.argument);
                break;
            case CMD_BENCHMARK:
                handleBenchmark();
                break;
            case CMD_HELP:
                Parser::printHelp();
                std::cout << "Additional commands:\n";
                std::cout << "  quests         - View quest chain in topological order\n";
                std::cout << "  catalog        - View item catalog stored in BST\n";
                break;
            case CMD_QUIT:
                std::cout << "Quitting game. Farewell adventurer!\n";
                gameOver = true;
                break;
            default:
                if (line.toLower() == "quests") {
                    handleQuests();
                } else if (line.toLower() == "catalog") {
                    handleCatalog();
                } else {
                    std::cout << "Unknown command. Type 'help' for guidance.\n";
                }
                break;
        }
    }
}
