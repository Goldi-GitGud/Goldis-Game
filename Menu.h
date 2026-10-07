#pragma once
#include <string>
#include "Game.h"
#include "Manager.h"

class Menu {
private:
    Game game;
    Manager manager;
    bool running = true;        // set to false by launch_exit so every menu loop ends
    bool leave_to_main = false; // set by "Return to Main Menu" in the pause menu

    bool choose_class(Player::PlayerClass& chosen); // false if the player backed out
    int choose_slot(const std::string& title);      // 1..Manager::SLOT_COUNT, or 0 if the player backed out
    void offer_save();                              // asks whether to save before leaving a game
    void play();                                    // runs the game, opening the pause menu whenever it pauses

public:
    void display_menu();
    void launch_game();
    bool launch_pause(); // returns true if the game should resume
    void launch_options();
    void launch_exit(int exit_code);
    bool difficulty();   // returns true if a difficulty was chosen, false if the player went back
};