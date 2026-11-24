// Graph.h - Module C (Ayesha Nehal)
// Dual representation room graph and graph algorithms:
// 1. Adjacency list and adjacency matrix representations
// 2. BFS for monster distance checking
// 3. DFS to verify dungeon room reachability
// 4. Dijkstra shortest path using MinHeap for monster chase
// 5. Kruskal Minimum Spanning Tree (MST) for corridor layout selection
// 6. Topological sort with cycle detection for quest ordering
#ifndef DATAQUEST_GRAPH_H
#define DATAQUEST_GRAPH_H

#include "../core/DynArray.h"
#include "../core/Queue.h"
#include "../core/Stack.h"
#include "../core/String.h"
#include "../tree/Heap.h"

struct Edge {
    int u, v;
    int weight;

    Edge() : u(0), v(0), weight(0) {}
    Edge(int u_, int v_, int w_) : u(u_), v(v_), weight(w_) {}

    bool operator==(const Edge& o) const {
        return (u == o.u && v == o.v && weight == o.weight) ||
               (u == o.v && v == o.u && weight == o.weight);
    }
};

// Disjoint Set Union (DSU) for Kruskal MST
class DSU {
private:
    int* parent;
    int* rank;
    int n;

public:
    explicit DSU(int size);
    ~DSU();

    int find(int i);
    bool unionSets(int i, int j);
};

class RoomGraph {
private:
    int numVertices;
    DynArray<Edge>* adjList;       // Adjacency list: array of DynArray<Edge>
    Array2D<int> adjMatrix;         // Adjacency matrix: weight or 0 if no edge

    void dfsHelper(int u, bool* visited, int& visitedCount) const;

public:
    explicit RoomGraph(int vertices = 0);
    ~RoomGraph();

    void init(int vertices);
    void addEdge(int u, int v, int weight = 1, bool undirected = true);
    bool hasEdge(int u, int v) const;
    int getWeight(int u, int v) const;
    int getVertexCount() const { return numVertices; }

    const DynArray<Edge>& getNeighbors(int u) const;
    const Array2D<int>& getAdjacencyMatrix() const { return adjMatrix; }

    // Graph Algorithms
    // BFS: computes shortest step distance from start vertex to all vertices
    void bfs(int start, DynArray<int>& distances) const;

    // DFS: verifies if every vertex is reachable from start vertex
    bool isFullyConnected(int start = 0) const;

    // Dijkstra: computes shortest path from start to target using Hamdan's MinHeap
    bool dijkstra(int start, int target, DynArray<int>& outPath, int& outTotalDist) const;

    // Kruskal MST: returns the minimum spanning tree edges
    static DynArray<Edge> kruskalMST(int vertices, const DynArray<Edge>& allEdges);

    void printAdjacencyMatrix(std::ostream& os = std::cout) const;
};

// Quest dependency graph for topological sort
struct Quest {
    int id;
    String name;
    String description;
    int rewardExp;

    Quest() : id(0), rewardExp(0) {}
    Quest(int id_, const String& name_, const String& desc_, int exp_)
        : id(id_), name(name_), description(desc_), rewardExp(exp_) {}
};

class QuestManager {
private:
    DynArray<Quest> quests;
    Array2D<bool> prereqMatrix;    // [u][v] == true means u must be done before v
    int questCount;

public:
    QuestManager();
    ~QuestManager();

    void addQuest(int id, const String& name, const String& desc, int exp);
    void addDependency(int prereqId, int targetId);

    // Topological sort: returns false if cycle detected (impossible quest chain)
    bool topologicalOrder(DynArray<Quest>& outOrder, String& errorMessage) const;

    int size() const { return questCount; }
    const Quest& getQuest(int id) const { return quests[id]; }
};

#endif
