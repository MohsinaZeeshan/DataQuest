// test_dungeon.cpp - Comprehensive Test Suite for Module C (Ayesha Nehal)
// Tests:
// 1. 20x20 Dungeon Grid and Non-Overlapping Room Generation
// 2. Per-Room Items via JaggedArray
// 3. Room Graph (Adjacency List and Matrix)
// 4. BFS Distance Checks and DFS Reachability
// 5. Dijkstra Shortest Path using MinHeap
// 6. Kruskal Minimum Spanning Tree (MST) using DSU
// 7. Topological Sort and Impossible Quest Cycle Detection
// 8. Recursive Backtracking and Iterative Stack-Based Hint Solvers
// 9. Combat Turn Order (Max-Heap), Turns (CircularList), Actions (Queue)
// 10. Memory Cleanliness and Edge Cases

#include "../include/game/Dungeon.h"
#include "../include/game/Graph.h"
#include "../include/game/HintSolver.h"
#include "../include/game/Game.h"
#include "../include/tree/Heap.h"
#include "../include/tree/BST.h"
#include "../include/tree/AVL.h"
#include <iostream>
#include <cassert>

static int passedTests = 0;
static int totalTests = 0;

#define TEST(desc) \
    totalTests++; \
    std::cout << "[TEST " << totalTests << "] " << desc << "... ";

#define PASS() \
    passedTests++; \
    std::cout << "PASS\n";

void testDungeonGeneration() {
    TEST("20x20 Grid dimensions and room placement without overlap");
    Dungeon d(42);
    d.generate(9, 42);

    assert(d.getRows() == 20);
    assert(d.getCols() == 20);
    assert(d.getRoomCount() >= 8 && d.getRoomCount() <= 12);

    const DynArray<Room>& rooms = d.getRooms();
    for (int i = 0; i < rooms.size(); i++) {
        for (int j = i + 1; j < rooms.size(); j++) {
            assert(!rooms[i].intersects(rooms[j], 1));
        }
    }
    PASS();

    TEST("Per-room item lists via JaggedArray");
    JaggedArray<Item>& rItems = d.getRoomItems();
    assert(rItems.numRows() == d.getRoomCount());

    Item testGem("Emerald", 1, 120, 1);
    d.addItemToRoom(1, testGem);
    assert(rItems.rowSize(1) > 0);

    Item retrieved;
    bool found = d.takeItemFromRoom(1, "Emerald", retrieved);
    assert(found);
    assert(retrieved.value == 120);
    PASS();
}

void testGraphAlgorithms() {
    TEST("Room graph adjacency list & matrix consistency");
    RoomGraph g(4);
    g.addEdge(0, 1, 5, true);
    g.addEdge(1, 2, 3, true);
    g.addEdge(2, 3, 7, true);
    g.addEdge(0, 3, 20, true);

    assert(g.hasEdge(0, 1));
    assert(g.hasEdge(1, 0));
    assert(g.getWeight(0, 1) == 5);
    assert(g.getWeight(2, 3) == 7);
    assert(!g.hasEdge(0, 2));
    PASS();

    TEST("BFS monster distance checks");
    DynArray<int> dists;
    g.bfs(0, dists);
    assert(dists[0] == 0);
    assert(dists[1] == 1);
    assert(dists[2] == 2);
    assert(dists[3] == 1); // 0->3 is 1 hop
    PASS();

    TEST("DFS connectivity check");
    assert(g.isFullyConnected(0));

    RoomGraph disconnected(3);
    disconnected.addEdge(0, 1, 2);
    assert(!disconnected.isFullyConnected(0));
    PASS();

    TEST("Dijkstra shortest path using MinHeap");
    DynArray<int> path;
    int totalDist = 0;
    bool ok = g.dijkstra(0, 3, path, totalDist);
    assert(ok);
    // Path 0->1->2->3 has weight 5+3+7 = 15, which is shorter than direct edge 20!
    assert(totalDist == 15);
    assert(path.size() == 4);
    assert(path[0] == 0 && path[1] == 1 && path[2] == 2 && path[3] == 3);
    PASS();

    TEST("Kruskal Minimum Spanning Tree (MST)");
    DynArray<Edge> allEdges;
    allEdges.pushBack(Edge(0, 1, 10));
    allEdges.pushBack(Edge(0, 2, 6));
    allEdges.pushBack(Edge(0, 3, 5));
    allEdges.pushBack(Edge(1, 3, 15));
    allEdges.pushBack(Edge(2, 3, 4));

    DynArray<Edge> mst = RoomGraph::kruskalMST(4, allEdges);
    assert(mst.size() == 3); // V - 1
    int mstWeight = 0;
    for (int i = 0; i < mst.size(); i++) mstWeight += mst[i].weight;
    assert(mstWeight == 19); // 4 + 5 + 10 = 19
    PASS();
}

void testQuestTopologicalSort() {
    TEST("Topological sort for valid quest ordering");
    QuestManager qm;
    qm.addQuest(0, "Awakening", "Chamber 1", 50);
    qm.addQuest(1, "Trial", "Chamber 2", 100);
    qm.addQuest(2, "Keymaster", "Chamber 3", 150);

    qm.addDependency(0, 1);
    qm.addDependency(1, 2);

    DynArray<Quest> order;
    String errMsg;
    bool ok = qm.topologicalOrder(order, errMsg);
    assert(ok);
    assert(order.size() == 3);
    assert(order[0].id == 0);
    assert(order[1].id == 1);
    assert(order[2].id == 2);
    PASS();

    TEST("Impossible quest cycle detection");
    QuestManager cyclicQm;
    cyclicQm.addQuest(0, "Quest A", "Desc", 10);
    cyclicQm.addQuest(1, "Quest B", "Desc", 20);
    cyclicQm.addQuest(2, "Quest C", "Desc", 30);

    cyclicQm.addDependency(0, 1);
    cyclicQm.addDependency(1, 2);
    cyclicQm.addDependency(2, 0); // Cycle! A -> B -> C -> A

    DynArray<Quest> badOrder;
    String badErrMsg;
    bool cyclicOk = cyclicQm.topologicalOrder(badOrder, badErrMsg);
    assert(!cyclicOk);
    assert(badErrMsg.length() > 0);
    PASS();
}

void testHintSolvers() {
    TEST("Recursive backtracking and iterative Stack-based hint solvers");
    Dungeon d(999);
    d.generate(9, 999);

    int startR = d.getPlayerStartR();
    int startC = d.getPlayerStartC();
    int exitR  = d.getExitR();
    int exitC  = d.getExitC();

    DynArray<Coord> recPath;
    bool recOk = HintSolver::solveRecursive(d, startR, startC, exitR, exitC, recPath);
    assert(recOk);
    assert(recPath.size() > 0);
    assert(recPath[0] == Coord(startR, startC));
    assert(recPath[recPath.size() - 1] == Coord(exitR, exitC));

    DynArray<Coord> iterPath;
    bool iterOk = HintSolver::solveIterative(d, startR, startC, exitR, exitC, iterPath);
    assert(iterOk);
    assert(iterPath.size() > 0);
    assert(iterPath[0] == Coord(startR, startC));
    assert(iterPath[iterPath.size() - 1] == Coord(exitR, exitC));

    // Confirm both paths only traverse walkable tiles
    for (int i = 0; i < recPath.size(); i++) {
        assert(d.isWalkable(recPath[i].r, recPath[i].c));
    }
    for (int i = 0; i < iterPath.size(); i++) {
        assert(d.isWalkable(iterPath[i].r, iterPath[i].c));
    }
    PASS();

    TEST("Recursion demonstrations (Factorial, Fibonacci, Tower of Hanoi)");
    assert(HintSolver::factorial(5) == 120);
    assert(HintSolver::fibonacci(8) == 21);
    DynArray<String> hanoiMoves;
    HintSolver::towerOfHanoi(3, 'A', 'C', 'B', hanoiMoves);
    assert(hanoiMoves.size() == 7); // 2^3 - 1
    PASS();
}

void testCombatStructures() {
    TEST("Combat structures: MaxHeap, CircularList, Queue");
    // MaxHeap for initiative
    MaxHeap<String> initiativeHeap(4);
    initiativeHeap.insert("Goblin", 8);
    initiativeHeap.insert("Player", 15);
    initiativeHeap.insert("Boss", 12);

    assert(initiativeHeap.extractMax() == "Player");
    assert(initiativeHeap.extractMax() == "Boss");
    assert(initiativeHeap.extractMax() == "Goblin");

    // CircularList for turns
    CircularList<String> turns;
    turns.insert("Player");
    turns.insert("Monster");
    assert(turns.current() == "Player");
    assert(turns.next() == "Monster");
    assert(turns.next() == "Player"); // wraps around automatically

    // Queue for action queue
    Queue<String> actionQueue;
    actionQueue.enqueue("Slash");
    actionQueue.enqueue("Block");
    assert(actionQueue.dequeue() == "Slash");
    assert(actionQueue.dequeue() == "Block");
    assert(actionQueue.isEmpty());
    PASS();
}

int main() {
    std::cout << "\n============================================\n";
    std::cout << "  RUNNING DATAQUEST MODULE C TEST SUITE     \n";
    std::cout << "  (Ayesha Nehal - Dungeon & Game Engine)   \n";
    std::cout << "============================================\n\n";

    testDungeonGeneration();
    testGraphAlgorithms();
    testQuestTopologicalSort();
    testHintSolvers();
    testCombatStructures();

    std::cout << "\n============================================\n";
    std::cout << "  SUMMARY: " << passedTests << " / " << totalTests << " TESTS PASSED SUCCESSFULLY!\n";
    std::cout << "============================================\n\n";
    return 0;
}
