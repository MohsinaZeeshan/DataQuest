<div align="center">

# DataQuest

### A terminal-based dungeon crawler in C++ built on custom data structures and algorithms

![Language](https://img.shields.io/badge/language-C%2B%2B-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey?style=flat-square)
![Interface](https://img.shields.io/badge/interface-terminal-2ea44f?style=flat-square)
![Dependencies](https://img.shields.io/badge/dependencies-none-blueviolet?style=flat-square)

[Overview](#overview) •
[Features](#features) •
[Getting Started](#getting-started) •
[How to Play](#how-to-play) •
[Architecture](#architecture) •
[Benchmark Mode](#benchmark-mode) •
[Contributing](#contributing)

</div>

---

## Overview

**DataQuest** is a turn-based roguelike played entirely in the terminal. Each game takes place in a newly generated 20 × 20 ASCII dungeon of 8–12 rooms connected by corridors. You explore, collect and use items, fight monsters that hunt you down, complete quests in the right order and find your way to the exit. Along the way you can undo moves, ask for hints, save your progress and compete for a place on the leaderboard.

Behind every game system is a data structure or algorithm implemented within the project, without STL containers. Monsters track you with Dijkstra's algorithm, the inventory is a hash table, undo and redo run on a stack, and the leaderboard is an AVL tree. A built-in **benchmark mode** times these algorithms against each other on the same data. Together they show how data structures, usually studied one at a time, work side by side in a real application.

<!-- Add a screenshot or GIF of gameplay here, e.g.:
<p align="center"><img src="docs/gameplay.gif" alt="DataQuest gameplay" width="600"></p>
-->

## Features

| Game system | Data structure / algorithm |
|---|---|
| **Dungeon map** | 2D dynamic grid; jagged arrays for per-room item lists |
| **Procedural generation** | Recursion and backtracking |
| **Rooms and corridors** | Graph (adjacency list and matrix); Kruskal's minimum spanning tree |
| **Monster AI** | Dijkstra's shortest path on a custom heap; BFS for distance checks |
| **Inventory** | Hash table with chaining, linear probing or double hashing; automatic rehashing |
| **Item search and sorting** | Linear, binary and interpolation search; sort by weight or value |
| **Item catalog** | Binary search tree with traversals and full/complete checks |
| **Combat turn order** | Circular linked list, max-heap priority queue and action queue |
| **Quest ordering** | Topological sort, with detection of impossible quest cycles |
| **Undo / redo** | Doubly linked list and stack |
| **Hint system** | Backtracking solver, in recursive and iterative (explicit stack) versions |
| **Leaderboard** | AVL tree; radix, merge and quick sort |
| **Save archive** | B-tree of configurable order (order 3 gives a 2-3 tree) |
| **Command parser and log search** | Brute force, Rabin-Karp, Boyer-Moore and KMP string matching |
| **Benchmark mode** | Side-by-side timing of algorithms on the same data |

## Getting Started

### Prerequisites

- A C++ compiler: **g++** on Linux, or **MinGW-w64** on Windows
- **make** (`mingw32-make` on Windows)

DataQuest has no external dependencies.

### Build and run

```bash
git clone https://github.com/MohsinaZeeshan/DataQuest.git
cd DataQuest
make
make run
```

| Command | Description |
|---|---|
| `make` | Build the game |
| `make run` | Build and launch |
| `make test` | Build and run the test suites |
| `make clean` | Remove build output |

## How to Play

You start somewhere in the dungeon. Explore the rooms, gather items, defeat or avoid the monsters, complete your quests and reach the exit.

| Command | Action |
|---|---|
| `move <direction>` | Move north, south, east or west |
| `look` | Describe the current room |
| `pickup <item>` / `drop <item>` | Manage items |
| `use <item>` | Use an item from the inventory |
| `attack` | Attack a nearby monster |
| `undo` / `redo` | Step backward or forward through your moves |
| `hint` | Show a path toward the current objective |
| `inventory` | List your items |
| `leaderboard` | Show the high scores |
| `save` / `load` | Save or restore a game |
| `benchmark` | Run benchmark mode |

## Architecture

DataQuest is organised into three independent modules. Each module exposes its functionality through header files that serve as a fixed contract, so every module can be developed and tested on its own.

```
┌─────────────────────────────────────────────────────────┐
│             Module C · Dungeon & Game Engine            │
│   grid · graphs · pathfinding · game loop · rendering   │
└──────────────┬───────────────────────────┬──────────────┘
               │                           │
┌──────────────▼──────────────┐ ┌──────────▼──────────────┐
│ Module A · Core Structures, │ │ Module B · Trees, Heaps,│
│     Strings & Commands      │ │     Search & Sort       │
└─────────────────────────────┘ └─────────────────────────┘
```

| Module | Scope |
|---|---|
| **A: Core Structures, Strings and Commands** | String class; dynamic and jagged arrays; singly, doubly and circular linked lists; stack and queue; hash tables; string-matching algorithms; command parser; undo/redo; save/load; benchmark harness |
| **B: Trees, Heaps, Search and Sort** | BST; AVL tree (all four rotations); B-tree; max-heap and heap sort; linear, binary and interpolation search; bubble, selection, insertion, comb, radix, merge and quick sort |
| **C: Dungeon and Game Engine** | Grid generation with backtracking; room graph; BFS, DFS, Dijkstra, Kruskal and topological sort; hint solver; game loop and ASCII display |

### Project layout

```
DataQuest/
├── include/       # Public headers (module interfaces)
│   ├── core/      # Module A
│   ├── trees/     # Module B
│   └── engine/    # Module C
├── src/           # Implementations, mirroring include/
├── tests/         # Test suites for each module
├── data/          # Save files, leaderboard and logs (created at runtime)
├── Makefile
└── README.md
```

### Design principles

- **No STL containers.** All containers and algorithms are implemented in the project. The standard library is used only for I/O, file handling, timing and random numbers.
- **Memory-safe.** Every allocation is owned and freed explicitly. Leaks are checked with Valgrind and AddressSanitizer.
- **Portable.** Standard C++ that builds on both Windows and Linux.

## Benchmark Mode

Benchmark mode runs each family of algorithms on identical input and prints a comparison table:

- **Sorting:** bubble, selection, insertion, comb, radix, merge, quick and heap sort
- **String matching:** brute force vs. Rabin-Karp vs. Boyer-Moore vs. KMP
- **Trees:** unbalanced BST vs. AVL tree on sorted and random insertions
- **Hashing:** chaining vs. linear probing vs. double hashing at different load factors

## Contributing

Contributions, bug reports and suggestions are welcome.

1. Fork the repository and create a branch for your change.
2. Keep the no-STL-containers rule for any data structure.
3. Make sure the project builds and `make test` passes.
4. Open a pull request that describes what you changed and why.

## Authors

- **Mohsina Zeeshan**: Core Structures, Strings and Commands
- **Muhammad Hamdan**: Trees, Heaps, Search and Sort
- **Ayesha Nehal**: Dungeon and Game Engine

## Acknowledgements

DataQuest was developed as the semester project for the **Data Structures** course (3rd semester) at the **National University of Computer and Emerging Sciences (FAST-NUCES), Karachi**.

It draws inspiration from classic roguelikes such as *Rogue*, *NetHack* and *Brogue*, and from algorithm-visualisation tools such as *VisuAlgo*.
