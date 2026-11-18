// SaveLoad.cpp - Module A (Mohsina Zeeshan)
#include "core/SaveLoad.h"
#include <fstream>

static const char* HEADER = "DATAQUEST_SAVE 1";

bool saveGame(const String& filename, const GameState& state, const HashTable& inventory, String& message) {
    std::ofstream out(filename.c_str());
    if (!out) {
        message = String("Could not open '") + filename + "' for writing.";
        return false;
    }
    DynArray<Item> items;
    inventory.toArray(items);

    out << HEADER << "\n";
    out << "NAME " << state.playerName << "\n";
    out << "STATE " << state.playerRow << " " << state.playerCol << " " << state.hp << " "
        << state.score << " " << state.turn << " " << state.seed << "\n";
    out << "ITEMS " << items.size() << "\n";
    for (int i = 0; i < items.size(); i++)
        out << items[i].name << "|" << items[i].weight << "|" << items[i].value << "|"
            << items[i].quantity << "\n";
    out << "END\n";

    if (!out) {
        message = String("Error while writing '") + filename + "'.";
        return false;
    }
    message = String("Game saved to '") + filename + "'.";
    return true;
}

// Small helper: fail with a "damaged file" message.
static bool damaged(String& message, const String& filename, const char* why) {
    message = String("Save file '") + filename + "' is damaged (" + why + "). Nothing was loaded.";
    return false;
}

bool loadGame(const String& filename, GameState& state, HashTable& inventory, String& message) {
    std::ifstream in(filename.c_str());
    if (!in) {
        message = String("Save file '") + filename + "' was not found.";
        return false;
    }

    String line;
    DynArray<String> parts;

    // 1. header
    if (!String::readLine(in, line) || !(line.trim() == HEADER))
        return damaged(message, filename, "bad header");

    // 2. NAME <name>
    GameState temp;
    if (!String::readLine(in, line) || line.find(String("NAME ")) != 0)
        return damaged(message, filename, "missing NAME");
    temp.playerName = line.substr(5).trim();

    // 3. STATE row col hp score turn seed
    if (!String::readLine(in, line)) return damaged(message, filename, "missing STATE");
    line.split(' ', parts);
    if (parts.size() != 7 || !(parts[0] == "STATE") ||
        !parts[1].toInt(temp.playerRow) || !parts[2].toInt(temp.playerCol) ||
        !parts[3].toInt(temp.hp) || !parts[4].toInt(temp.score) ||
        !parts[5].toInt(temp.turn) || !parts[6].toInt(temp.seed))
        return damaged(message, filename, "bad STATE line");

    // 4. ITEMS count
    int itemCount = 0;
    if (!String::readLine(in, line)) return damaged(message, filename, "missing ITEMS");
    line.split(' ', parts);
    if (parts.size() != 2 || !(parts[0] == "ITEMS") || !parts[1].toInt(itemCount) || itemCount < 0)
        return damaged(message, filename, "bad ITEMS line");

    // 5. each item: name|weight|value|qty
    DynArray<Item> items;
    for (int i = 0; i < itemCount; i++) {
        if (!String::readLine(in, line)) return damaged(message, filename, "fewer items than expected");
        line.split('|', parts, false);
        Item it;
        if (parts.size() != 4 || parts[0].trim().empty() ||
            !parts[1].toInt(it.weight) || !parts[2].toInt(it.value) || !parts[3].toInt(it.quantity))
            return damaged(message, filename, "bad item line");
        it.name = parts[0].trim();
        items.pushBack(it);
    }

    // 6. END
    if (!String::readLine(in, line) || !(line.trim() == "END"))
        return damaged(message, filename, "missing END");

    // Everything is valid: now replace the current game.
    state = temp;
    inventory.clear();
    for (int i = 0; i < items.size(); i++) inventory.addItem(items[i]);
    message = String("Game loaded from '") + filename + "'.";
    return true;
}
