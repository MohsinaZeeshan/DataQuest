// Parser.h - Module A (Mohsina Zeeshan)
// Turns a line the player typed into a Command, and keeps a searchable game log.
#ifndef DATAQUEST_PARSER_H
#define DATAQUEST_PARSER_H

#include "String.h"
#include "DynArray.h"
#include "StringMatch.h"

enum CommandType {
    CMD_MOVE, CMD_LOOK, CMD_PICKUP, CMD_DROP, CMD_USE, CMD_ATTACK,
    CMD_UNDO, CMD_REDO, CMD_HINT, CMD_INVENTORY, CMD_LEADERBOARD,
    CMD_SAVE, CMD_LOAD, CMD_BENCHMARK, CMD_SEARCH, CMD_HELP, CMD_QUIT,
    CMD_EMPTY,      // the player just pressed Enter
    CMD_UNKNOWN     // not a command (message holds the error / suggestion)
};

struct Command {
    CommandType type;
    String argument;   // "north", "rusty sword", "goblin", "savegame.txt", search word ...
    bool valid;        // false if the command was known but used wrongly
    String message;    // error text to show the player when !valid or CMD_UNKNOWN

    Command() : type(CMD_EMPTY), valid(true) {}
};

class Parser {
public:
    // Examples:
    //   "move north" / "go n" / "n"  -> CMD_MOVE, argument "north"
    //   "pick up Rusty Sword"        -> CMD_PICKUP, argument "rusty sword"
    //   "search goblin"              -> CMD_SEARCH, argument "goblin"
    //                                   (game calls log.search(cmd.argument, KMP))
    //   "mvoe north"                 -> CMD_UNKNOWN, "Did you mean 'move'?"
    static Command parse(const String& line);
    static const char* commandName(CommandType type);
    static void printHelp();

    // Levenshtein edit distance (dynamic programming on our Array2D). O(a*b)
    static int editDistance(const String& a, const String& b);

private:
    static String normaliseDirection(const String& word);   // "" if not a direction
    static String joinWords(const DynArray<String>& words, int from);
    static String suggest(const String& word);
};

// Game log: every message the game prints can be added here, then searched.
class GameLog {
private:
    DynArray<String> entries;

public:
    void add(const String& entry) { entries.pushBack(entry); }
    int size() const { return entries.size(); }
    const String& get(int i) const { return entries[i]; }
    void print() const;
    // Prints every log line that contains `word` (case-insensitive) using the
    // chosen matcher. Returns how many lines matched.
    int search(const String& word, MatchAlgorithm algo, bool printResults = true) const;
    void clear() { entries.clear(); }
};

#endif
