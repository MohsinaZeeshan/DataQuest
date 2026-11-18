// History.h - Module A (Mohsina Zeeshan)
// Undo / redo of player moves.
//   done      : doubly linked list of moves made (newest at the back)
//   redoStack : moves that were undone and can be redone
// A new move clears the redo stack (you can't redo after changing the past).
// recordMove / undo / redo are all O(1).
#ifndef DATAQUEST_HISTORY_H
#define DATAQUEST_HISTORY_H

#include "LinkedList.h"
#include "Stack.h"
#include <iostream>

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;

    Move() : fromRow(0), fromCol(0), toRow(0), toCol(0) {}
    Move(int fr, int fc, int tr, int tc) : fromRow(fr), fromCol(fc), toRow(tr), toCol(tc) {}
    bool operator==(const Move& o) const {
        return fromRow == o.fromRow && fromCol == o.fromCol && toRow == o.toRow && toCol == o.toCol;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Move& m) {
    os << "(" << m.fromRow << "," << m.fromCol << ")->(" << m.toRow << "," << m.toCol << ")";
    return os;
}

class MoveHistory {
private:
    DoublyLinkedList<Move> done;
    Stack<Move> redoStack;
    int maxMoves;      // oldest moves are dropped after this many

public:
    explicit MoveHistory(int maxMoves = 100) : maxMoves(maxMoves) {}

    void recordMove(const Move& m) {
        done.pushBack(m);
        if (done.size() > maxMoves) done.popFront();   // forget the oldest move
        redoStack.clear();
    }

    // On success, `out` is the move that was undone: put the player back at
    // (out.fromRow, out.fromCol).
    bool undo(Move& out) {
        if (done.isEmpty()) return false;
        out = done.popBack();
        redoStack.push(out);
        return true;
    }

    // On success, `out` is the move to repeat: put the player at (out.toRow, out.toCol).
    bool redo(Move& out) {
        if (redoStack.isEmpty()) return false;
        out = redoStack.pop();
        done.pushBack(out);
        return true;
    }

    bool canUndo() const { return !done.isEmpty(); }
    bool canRedo() const { return !redoStack.isEmpty(); }
    int movesMade() const { return done.size(); }
    int redoAvailable() const { return redoStack.size(); }

    void printHistory() const {
        std::cout << "Moves (oldest first): ";
        if (done.isEmpty()) std::cout << "none\n";
        else done.printForward();
    }

    void clear() {
        done.clear();
        redoStack.clear();
    }
};

#endif
