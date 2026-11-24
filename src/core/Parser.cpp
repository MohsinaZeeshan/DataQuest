// Parser.cpp - Module A (Mohsina Zeeshan)
#include "core/Parser.h"
#include <iostream>

// Words we can suggest when the player misspells a command.
static const char* KNOWN_COMMANDS[] = {
    "move", "look", "pickup", "drop", "use", "attack", "undo", "redo", "hint",
    "inventory", "leaderboard", "save", "load", "benchmark", "search", "quests",
    "catalog", "archive", "help", "quit"
};
static const int NUM_KNOWN = 20;

String Parser::normaliseDirection(const String& w) {
    if (w == "n" || w == "north") return "north";
    if (w == "s" || w == "south") return "south";
    if (w == "e" || w == "east")  return "east";
    if (w == "w" || w == "west")  return "west";
    return "";
}

String Parser::joinWords(const DynArray<String>& words, int from) {
    String result;
    for (int i = from; i < words.size(); i++) {
        if (i > from) result += ' ';
        result += words[i];
    }
    return result;
}

int Parser::editDistance(const String& a, const String& b) {
    int n = a.length(), m = b.length();
    // dp(i, j) = edits to turn the first i chars of a into the first j chars of b
    Array2D<int> dp(n + 1, m + 1, 0);
    for (int i = 0; i <= n; i++) dp.at(i, 0) = i;
    for (int j = 0; j <= m; j++) dp.at(0, j) = j;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            int best = dp.at(i - 1, j) + 1;                                  // delete
            if (dp.at(i, j - 1) + 1 < best) best = dp.at(i, j - 1) + 1;      // insert
            if (dp.at(i - 1, j - 1) + cost < best) best = dp.at(i - 1, j - 1) + cost; // replace
            dp.at(i, j) = best;
        }
    }
    return dp.at(n, m);
}

String Parser::suggest(const String& word) {
    int bestDist = 3;          // only suggest if 2 edits or fewer
    String best;
    for (int i = 0; i < NUM_KNOWN; i++) {
        int d = editDistance(word, KNOWN_COMMANDS[i]);
        if (d < bestDist) {
            bestDist = d;
            best = KNOWN_COMMANDS[i];
        }
    }
    if (best == "pickup") best = "pick up";
    return best;
}

Command Parser::parse(const String& line) {
    Command cmd;
    DynArray<String> words;
    line.toLower().trim().split(' ', words);   // lowercase, split on spaces

    if (words.size() == 0) {
        cmd.type = CMD_EMPTY;
        return cmd;
    }

    const String& w = words[0];

    // ---- movement: "move north", "go n", or just "north" / "n"
    if (w == "move" || w == "go") {
        cmd.type = CMD_MOVE;
        if (words.size() < 2) {
            cmd.valid = false;
            cmd.message = "Move where? Try: move north / south / east / west";
            return cmd;
        }
        cmd.argument = normaliseDirection(words[1]);
        if (cmd.argument.empty()) {
            cmd.valid = false;
            cmd.message = String("'") + words[1] + "' is not a direction. Use north, south, east or west.";
        }
        return cmd;
    }
    if (!normaliseDirection(w).empty() && words.size() == 1) {
        cmd.type = CMD_MOVE;
        cmd.argument = normaliseDirection(w);
        return cmd;
    }

    // ---- item commands
    if (w == "pick" || w == "pickup" || w == "take") {
        cmd.type = CMD_PICKUP;
        int start = 1;
        if (w == "pick") {
            if (words.size() < 2 || !(words[1] == "up")) {
                cmd.valid = false;
                cmd.message = "Did you mean 'pick up <item>'?";
                return cmd;
            }
            start = 2;
        }
        cmd.argument = joinWords(words, start);
        if (cmd.argument.empty()) {
            cmd.valid = false;
            cmd.message = "Pick up what? Try: pick up <item>";
        }
        return cmd;
    }
    if (w == "drop" || w == "use") {
        cmd.type = (w == "drop") ? CMD_DROP : CMD_USE;
        cmd.argument = joinWords(words, 1);
        if (cmd.argument.empty()) {
            cmd.valid = false;
            cmd.message = String(w == "drop" ? "Drop" : "Use") + " what? Try: " + w + " <item>";
        }
        return cmd;
    }

    // ---- search the game log: "search goblin"
    if (w == "search" || w == "find") {
        cmd.type = CMD_SEARCH;
        cmd.argument = joinWords(words, 1);
        if (cmd.argument.empty()) {
            cmd.valid = false;
            cmd.message = "Search for what? Try: search <word>";
        }
        return cmd;
    }

    // ---- commands with an optional argument
    if (w == "attack" || w == "fight") {
        cmd.type = CMD_ATTACK;
        cmd.argument = joinWords(words, 1);          // monster name, may be empty
        return cmd;
    }
    if (w == "save" || w == "load") {
        cmd.type = (w == "save") ? CMD_SAVE : CMD_LOAD;
        cmd.argument = joinWords(words, 1);
        if (cmd.argument.empty()) cmd.argument = "data/savegame.txt";
        return cmd;
    }

    // ---- single-word commands
    if (w == "look" || w == "l")                  { cmd.type = CMD_LOOK; return cmd; }
    if (w == "undo")                              { cmd.type = CMD_UNDO; return cmd; }
    if (w == "redo")                              { cmd.type = CMD_REDO; return cmd; }
    if (w == "hint")                              { cmd.type = CMD_HINT; return cmd; }
    if (w == "inventory" || w == "inv" || w == "i") {
        cmd.type = CMD_INVENTORY;
        cmd.argument = joinWords(words, 1);      // optional: "weight" or "value" to sort
        if (!cmd.argument.empty() && !(cmd.argument == "weight") && !(cmd.argument == "value")) {
            cmd.valid = false;
            cmd.message = "Sort the inventory by what? Try: inventory weight / inventory value";
        }
        return cmd;
    }
    if (w == "archive" || w == "saves") {        // optional save ID to look up
        cmd.type = CMD_ARCHIVE;
        cmd.argument = joinWords(words, 1);
        return cmd;
    }
    if (w == "quests" || w == "quest")            { cmd.type = CMD_QUESTS; return cmd; }
    if (w == "catalog")                           { cmd.type = CMD_CATALOG; return cmd; }
    if (w == "leaderboard" || w == "scores")      { cmd.type = CMD_LEADERBOARD; return cmd; }
    if (w == "benchmark")                         { cmd.type = CMD_BENCHMARK; return cmd; }
    if (w == "help" || w == "?")                  { cmd.type = CMD_HELP; return cmd; }
    if (w == "quit" || w == "exit")               { cmd.type = CMD_QUIT; return cmd; }

    // ---- unknown: suggest the closest command
    cmd.type = CMD_UNKNOWN;
    cmd.valid = false;
    String guess = suggest(w);
    cmd.message = String("Unknown command '") + w + "'.";
    if (!guess.empty()) cmd.message += String(" Did you mean '") + guess + "'?";
    else cmd.message += " Type 'help' to see all commands.";
    return cmd;
}

const char* Parser::commandName(CommandType type) {
    switch (type) {
        case CMD_MOVE: return "move";               case CMD_LOOK: return "look";
        case CMD_PICKUP: return "pick up";          case CMD_DROP: return "drop";
        case CMD_USE: return "use";                 case CMD_ATTACK: return "attack";
        case CMD_UNDO: return "undo";               case CMD_REDO: return "redo";
        case CMD_HINT: return "hint";               case CMD_INVENTORY: return "inventory";
        case CMD_LEADERBOARD: return "leaderboard"; case CMD_SAVE: return "save";
        case CMD_LOAD: return "load";               case CMD_BENCHMARK: return "benchmark";
        case CMD_SEARCH: return "search";           case CMD_QUESTS: return "quests";
        case CMD_CATALOG: return "catalog";         case CMD_ARCHIVE: return "archive";
        case CMD_HELP: return "help";               case CMD_QUIT: return "quit";
        case CMD_EMPTY: return "(empty)";           case CMD_UNKNOWN: return "(unknown)";
    }
    return "?";
}

void Parser::printHelp() {
    std::cout <<
        "Commands:\n"
        "  move <north|south|east|west>   (or just n / s / e / w)\n"
        "  look                           describe the room\n"
        "  pick up <item>   drop <item>   use <item>\n"
        "  attack [monster]\n"
        "  undo   redo   hint\n"
        "  inventory [weight|value]       list items (optionally sorted)\n"
        "  leaderboard   quests   catalog   benchmark\n"
        "  save [file]   load [file]   archive [save id]\n"
        "  search <word>                  search the game log\n"
        "  help   quit\n";
}

// ---------- GameLog ----------

void GameLog::print() const {
    for (int i = 0; i < entries.size(); i++)
        std::cout << "[" << i << "] " << entries[i] << "\n";
}

int GameLog::search(const String& word, MatchAlgorithm algo, bool printResults) const {
    MatchFunction match = getMatcher(algo);
    String pattern = word.toLower();
    DynArray<int> positions;
    int linesFound = 0;
    for (int i = 0; i < entries.size(); i++) {
        if (match(entries[i].toLower(), pattern, positions) > 0) {
            linesFound++;
            if (printResults) std::cout << "[" << i << "] " << entries[i] << "\n";
        }
    }
    if (printResults && linesFound == 0)
        std::cout << "'" << word << "' was not found in the log.\n";
    return linesFound;
}
