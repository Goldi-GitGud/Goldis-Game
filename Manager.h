#pragma once
#include <string>
#include "Game.h"

// Saves and loads a Game to/from JSON files, one file per slot (save_slot_1.json ...).
class Manager {
public:
    static constexpr int SLOT_COUNT = 3;

    // Both return true on success and print a message themselves when something goes wrong.
    bool save_game_saves(const Game& game, int slot) const;
    bool load_game_saves(Game& game, int slot) const;
    // One line for the slot list, e.g. "Warrior, level 3, wave 4" or "empty"
    std::string describe_slot(int slot) const;

private:
    static std::string slot_path(int slot);
};