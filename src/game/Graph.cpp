// Graph.cpp - Module C (Ayesha Nehal)
#include "../../include/game/Graph.h"
#include <iostream>

// ============================================================
// Disjoint Set Union (DSU) for Kruskal MST
// ============================================================
DSU::DSU(int size) : n(size) {
    parent = new int[n];
    rank = new int[n];
    for (int i = 0; i < n; i++) {
        parent[i] = i;
        rank[i] = 0;
    }
}

DSU::~DSU() {
    delete[] parent;
    delete[] rank;
}

int DSU::find(int i) {
    if (parent[i] == i) return i;
    return parent[i] = find(parent[i]); // path compression
}

bool DSU::unionSets(int i, int j) {
    int rootI = find(i);
    int rootJ = find(j);
    if (rootI == rootJ) return false; // cycle detected

    // union by rank
    if (rank[rootI] < rank[rootJ]) {
        parent[rootI] = rootJ;
    } else if (rank[rootI] > rank[rootJ]) {
        parent[rootJ] = rootI;
    } else {
        parent[rootJ] = rootI;
        rank[rootI]++;
    }
    return true;
}

// ============================================================
// RoomGraph (Adjacency List + Adjacency Matrix)
// ============================================================
RoomGraph::RoomGraph(int vertices) : numVertices(0), adjList(nullptr), adjMatrix(0, 0, 0) {
    if (vertices > 0) init(vertices);
}

RoomGraph::~RoomGraph() {
    delete[] adjList;
}

void RoomGraph::init(int vertices) {
    delete[] adjList;
    numVertices = vertices;
    adjList = new DynArray<Edge>[numVertices];
    adjMatrix.resize(numVertices, numVertices, 0);
}

void RoomGraph::addEdge(int u, int v, int weight, bool undirected) {
    if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) return;

    adjList[u].pushBack(Edge(u, v, weight));
    adjMatrix.at(u, v) = weight;

    if (undirected && u != v) {
        adjList[v].pushBack(Edge(v, u, weight));
        adjMatrix.at(v, u) = weight;
    }
}

bool RoomGraph::hasEdge(int u, int v) const {
    if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) return false;
    return adjMatrix.at(u, v) != 0;
}

int RoomGraph::getWeight(int u, int v) const {
    if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) return 0;
    return adjMatrix.at(u, v);
}

const DynArray<Edge>& RoomGraph::getNeighbors(int u) const {
    if (u < 0 || u >= numVertices) {
        static DynArray<Edge> emptyList;
        return emptyList;
    }
    return adjList[u];
}

// BFS: computes shortest step distance from start vertex to all vertices
void RoomGraph::bfs(int start, DynArray<int>& distances) const {
    distances.clear();
    for (int i = 0; i < numVertices; i++) distances.pushBack(-1);
    if (start < 0 || start >= numVertices) return;

    Queue<int> q;
    distances[start] = 0;
    q.enqueue(start);

    while (!q.isEmpty()) {
        int u = q.dequeue();
        const DynArray<Edge>& neighbors = adjList[u];
        for (int i = 0; i < neighbors.size(); i++) {
            int v = neighbors[i].v;
            if (distances[v] == -1) {
                distances[v] = distances[u] + 1;
                q.enqueue(v);
            }
        }
    }
}

void RoomGraph::dfsHelper(int u, bool* visited, int& visitedCount) const {
    visited[u] = true;
    visitedCount++;

    const DynArray<Edge>& neighbors = adjList[u];
    for (int i = 0; i < neighbors.size(); i++) {
        int v = neighbors[i].v;
        if (!visited[v]) {
            dfsHelper(v, visited, visitedCount);
        }
    }
}

// DFS: verifies if every vertex is reachable from start vertex
bool RoomGraph::isFullyConnected(int start) const {
    if (numVertices == 0) return true;
    if (start < 0 || start >= numVertices) return false;

    bool* visited = new bool[numVertices];
    for (int i = 0; i < numVertices; i++) visited[i] = false;

    int visitedCount = 0;
    dfsHelper(start, visited, visitedCount);

    delete[] visited;
    return visitedCount == numVertices;
}

// Dijkstra shortest path using Hamdan's MinHeap
bool RoomGraph::dijkstra(int start, int target, DynArray<int>& outPath, int& outTotalDist) const {
    outPath.clear();
    outTotalDist = -1;
    if (start < 0 || start >= numVertices || target < 0 || target >= numVertices) return false;
    if (start == target) {
        outPath.pushBack(start);
        outTotalDist = 0;
        return true;
    }

    const int INF = 1000000000;
    int* dist = new int[numVertices];
    int* prev = new int[numVertices];
    bool* visited = new bool[numVertices];

    for (int i = 0; i < numVertices; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = false;
    }

    dist[start] = 0;
    MinHeap<int> minHeap(numVertices * 2);
    minHeap.insert(start, 0);

    while (!minHeap.isEmpty()) {
        int u = minHeap.peek();
        minHeap.extractMin();

        if (visited[u]) continue;
        visited[u] = true;

        if (u == target) break;

        const DynArray<Edge>& neighbors = adjList[u];
        for (int i = 0; i < neighbors.size(); i++) {
            int v = neighbors[i].v;
            int w = neighbors[i].weight;
            if (!visited[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                minHeap.insert(v, dist[v]);
            }
        }
    }

    bool reachable = (dist[target] != INF);
    if (reachable) {
        outTotalDist = dist[target];
        // Reconstruct path
        DynArray<int> revPath;
        int curr = target;
        while (curr != -1) {
            revPath.pushBack(curr);
            curr = prev[curr];
        }
        for (int i = revPath.size() - 1; i >= 0; i--) {
            outPath.pushBack(revPath[i]);
        }
    }

    delete[] dist;
    delete[] prev;
    delete[] visited;
    return reachable;
}

// Kruskal MST: returns the minimum spanning tree edges
DynArray<Edge> RoomGraph::kruskalMST(int vertices, const DynArray<Edge>& allEdges) {
    DynArray<Edge> mst;
    if (vertices <= 1 || allEdges.isEmpty()) return mst;

    // Make a copy of edges to sort
    DynArray<Edge> sortedEdges = allEdges;
    // Insertion sort by weight
    for (int i = 1; i < sortedEdges.size(); i++) {
        Edge key = sortedEdges[i];
        int j = i - 1;
        while (j >= 0 && sortedEdges[j].weight > key.weight) {
            sortedEdges[j + 1] = sortedEdges[j];
            j--;
        }
        sortedEdges[j + 1] = key;
    }

    DSU dsu(vertices);
    for (int i = 0; i < sortedEdges.size(); i++) {
        const Edge& e = sortedEdges[i];
        if (dsu.unionSets(e.u, e.v)) {
            mst.pushBack(e);
            if (mst.size() == vertices - 1) break;
        }
    }
    return mst;
}

void RoomGraph::printAdjacencyMatrix(std::ostream& os) const {
    os << "Room Graph Adjacency Matrix (" << numVertices << "x" << numVertices << "):\n   ";
    for (int j = 0; j < numVertices; j++) os << " " << j;
    os << "\n";
    for (int i = 0; i < numVertices; i++) {
        os << (i < 10 ? " " : "") << i << ":";
        for (int j = 0; j < numVertices; j++) {
            os << " " << adjMatrix.at(i, j);
        }
        os << "\n";
    }
}

// ============================================================
// QuestManager (Topological Sort + Cycle Detection)
// ============================================================
QuestManager::QuestManager() : prereqMatrix(0, 0, false), questCount(0) {
}

QuestManager::~QuestManager() {
}

void QuestManager::addQuest(int id, const String& name, const String& desc, int exp) {
    quests.pushBack(Quest(id, name, desc, exp));
    questCount = quests.size();
    prereqMatrix.resize(questCount, questCount, false);
}

void QuestManager::addDependency(int prereqId, int targetId) {
    if (prereqId >= 0 && prereqId < questCount && targetId >= 0 && targetId < questCount) {
        prereqMatrix.at(prereqId, targetId) = true;
    }
}

bool QuestManager::topologicalOrder(DynArray<Quest>& outOrder, String& errorMessage) const {
    outOrder.clear();
    errorMessage = "";
    if (questCount == 0) return true;

    // Calculate in-degree
    int* inDegree = new int[questCount];
    for (int i = 0; i < questCount; i++) inDegree[i] = 0;

    for (int u = 0; u < questCount; u++) {
        for (int v = 0; v < questCount; v++) {
            if (prereqMatrix.at(u, v)) {
                inDegree[v]++;
            }
        }
    }

    Queue<int> q;
    for (int i = 0; i < questCount; i++) {
        if (inDegree[i] == 0) {
            q.enqueue(i);
        }
    }

    int visited = 0;
    while (!q.isEmpty()) {
        int u = q.dequeue();
        outOrder.pushBack(quests[u]);
        visited++;

        for (int v = 0; v < questCount; v++) {
            if (prereqMatrix.at(u, v)) {
                inDegree[v]--;
                if (inDegree[v] == 0) {
                    q.enqueue(v);
                }
            }
        }
    }

    delete[] inDegree;

    if (visited < questCount) {
        errorMessage = "Impossible quest cycle detected: quest requirements contain a circular dependency!";
        outOrder.clear();
        return false;
    }

    return true;
}
