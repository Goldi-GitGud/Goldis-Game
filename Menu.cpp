#include "Menu.h"
#include "Utils.h"
#include <iostream>
using namespace std;

void Menu::display_menu() {
    while (running) {
        cout << "Welcome to Goldi's Game!" << endl;
        cout << "___________________________________________" << endl;
        cout << "1. Start Game" << endl;
        cout << "2. Options" << endl;
        cout << "3. Exit" << endl;
        cout << "___________________________________________" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 3);
        switch (choice)
        {
        case 1:
            launch_game();
            break;
        case 2:
            launch_options();
            break;
        case 3:
            launch_exit(0);
            break;
        }
    }
}

void Menu::launch_game() {
    while (running) {
        cout << "Welcome to the Game Menu!" << endl;
        cout << "____________________________________________" << endl;
        cout << "1. New Game" << endl;
        cout << "2. Load Game" << endl;
        cout << "3. Return to Main Menu" << endl;
        cout << "___________________________________________" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 3);
        switch (choice)
        {
        case 1: {
            Player::PlayerClass chosen_class;
            if (!choose_class(chosen_class)) break;
            if (!difficulty()) break;
            game.new_game(chosen_class);
            cout << "Starting Game..." << endl;
            play();
            break;
        }
        case 2: {
            int slot = choose_slot("Load which save?");
            if (slot != 0 && manager.load_game_saves(game, slot)) {
                play();
            }
            break;
        }
        case 3:
            return;
        }
        // "Return to Main Menu" from the pause menu ends up here
        if (leave_to_main) {
            leave_to_main = false;
            return;
        }
    }
}

// Runs the game. Every time it pauses, the pause menu decides whether to resume or leave.
// When the player dies the game prints its own "GAME OVER" summary and we just return.
void Menu::play() {
    Game::Result result = game.run();
    while (result == Game::Result::Paused) {
        if (!launch_pause()) {
            return;
        }
        result = game.run();
    }
}

bool Menu::launch_pause() {
    while (true) {
        cout << "____________________________________________" << endl;
        cout << "1. Resume Game" << endl;
        cout << "2. Save Game" << endl;
        cout << "3. Change Difficulty" << endl;
        cout << "4. Return to Main Menu" << endl;
        cout << "5. Exit Game" << endl;
        cout << "___________________________________________" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 5);
        switch (choice)
        {
        case 1:
            return true;
        case 2: {
            int slot = choose_slot("Save to which slot?");
            if (slot != 0) manager.save_game_saves(game, slot);
            break;
        }
        case 3:
            difficulty();
            break;
        case 4:
            offer_save();
            leave_to_main = true;
            return false;
        case 5:
            offer_save();
            launch_exit(0);
            return false;
        }
    }
}

void Menu::launch_options() {
    while (true) {
        cout << "____________________________________________" << endl;
        cout << "1. Change Difficulty" << endl;
        cout << "2. Return to Main Menu" << endl;
        cout << "____________________________________________" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 2);
        switch (choice)
        {
        case 1:
            difficulty();
            break;
        case 2:
            return;
        }
    }
}

void Menu::launch_exit(int code) {
    if (code == 0) {
        cout << "Thank you for playing the game!" << endl;
    }
    else {
        cout << "Invalid choice. Exiting the program." << endl;
    }
    running = false;
}

bool Menu::difficulty() {
    cout << "____________________________________________" << endl;
    cout << "Current difficulty level: " << game.get_difficulty_name() << endl;
    cout << " choose difficulty level: " << endl;
    cout << "1. Easy (The enemies are weaker and you are a bit more stronger)" << endl;
    cout << "2. Normal (Balanced difficulty , recommended for most players)" << endl;
    cout << "3. Hard (The enemies are significantly stronger , recommended for experienced players)" << endl;
    cout << "4. Return to Previous Menu" << endl;
    cout << "Note: This will affect all saves and can be changed from the options menu." << endl;
    cout << "____________________________________________" << endl;
    cout << "Enter your choice: ";
    int choice = read_choice(1, 4);
    if (choice == 4) {
        return false;
    }
    game.set_difficulty(choice);
    cout << "Difficulty level set to " << game.get_difficulty_name() << "." << endl;
    return true;
}

// ---- helpers ----

bool Menu::choose_class(Player::PlayerClass& chosen) {
    const int class_count = static_cast<int>(Player::PlayerClass::Hunter) + 1; // Hunter is the last class in the enum
    cout << "____________________________________________" << endl;
    cout << "Choose your class:" << endl;
    for (int i = 0; i < class_count; i++) {
        Player::PlayerClass pc = static_cast<Player::PlayerClass>(i);
        Player preview(pc); // loads the class's stats from Player_data.json just to show them
        cout << i + 1 << ". " << Player::class_to_string(pc)
             << "  (HP " << preview.get_health() << ", Attack " << preview.get_attack_power()
             << ", Armor " << preview.get_armor() << ", Dodge " << preview.get_dodge_chance()
             << "%, Weapon: " << preview.get_weapon().get_name() << ")" << endl;
    }
    cout << "0. Back" << endl;
    cout << "Enter your choice: ";
    int choice = read_choice(0, class_count);
    if (choice == 0) {
        return false;
    }
    chosen = static_cast<Player::PlayerClass>(choice - 1);
    return true;
}

int Menu::choose_slot(const string& title) {
    cout << "____________________________________________" << endl;
    cout << title << endl;
    for (int slot = 1; slot <= Manager::SLOT_COUNT; slot++) {
        cout << slot << ". Slot " << slot << ": " << manager.describe_slot(slot) << endl;
    }
    cout << "0. Back" << endl;
    cout << "Enter your choice: ";
    return read_choice(0, Manager::SLOT_COUNT);
}

void Menu::offer_save() {
    cout << "Save before leaving?" << endl;
    cout << "1. Yes" << endl;
    cout << "2. No" << endl;
    cout << "Enter your choice: ";
    if (read_choice(1, 2) == 1) {
        int slot = choose_slot("Save to which slot?");
        if (slot != 0) manager.save_game_saves(game, slot);
    }
}