#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <stdexcept>
using namespace std;

Game::Game() : wave(1), difficulty(2) {}

void Game::new_game(Player::PlayerClass pc) {
    combat.reset(); // Combat points at the player and bag, so it has to go first
    player.emplace(pc);
    bag = Bag();
    // placeholder starting items so "Use item" has something to use
    bag.addItem(Item(Item::ItemType::Small_Health_potion));
    bag.addItem(Item(Item::ItemType::Small_Health_potion));
    bag.addItem(Item(Item::ItemType::Small_Defense_potion));
    wave = 1;
    combat.emplace(*player, bag);
}

void Game::load_state(const Player& p, const Bag& b, int wave_number) {
    combat.reset();
    player.emplace(p);
    bag = b;
    wave = wave_number;
    combat.emplace(*player, bag);
}

bool Game::has_game() const {
    return player.has_value();
}

// ---- the menu between waves ----

bool Game::camp() {
    while (true) {
        cout << "____________________________________________" << endl;
        cout << "Camp - next up: wave " << wave << "   (HP: " << player->get_health() << ")" << endl;
        cout << "1. Start wave" << endl;
        cout << "2. Use item" << endl;
        cout << "3. View my stats" << endl;
        cout << "4. View bag" << endl;
        cout << "5. Pause" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 5);
        switch (choice) {
        case 1:
            return true;
        case 2:
            combat->use_item_menu();
            break;
        case 3:
            player->display_stats();
            break;
        case 4:
            bag.displayContents();
            break;
        default:
            return false;
        }
    }
}

// ---- the wave loop ----

Game::Result Game::run() {
    if (!has_game()) {
        throw logic_error("Game::run called before new_game or load_state");
    }
    while (true) {
        // After pausing in the middle of a wave we go straight back into the fight instead of the camp
        if (!combat->is_wave_active()) {
            if (!camp()) {
                return Result::Paused;
            }
        }
        Combat::Result result = combat->fight(wave, difficulty);
        if (result == Combat::Result::Paused) {
            return Result::Paused;
        }
        if (result == Combat::Result::PlayerDied) {
            cout << "____________________________________________" << endl;
            cout << "GAME OVER - you reached wave " << wave << " as a level " << player->get_level()
                 << " " << player->Type_to_string() << " with " << player->get_gold() << " gold." << endl;
            cout << "____________________________________________" << endl;
            combat.reset();
            player.reset();
            return Result::GameOver;
        }
        wave++; // wave cleared: back to camp for the next one
    }
}

// ---- settings and read access ----

int Game::get_difficulty() const {
    return difficulty;
}
string Game::get_difficulty_name() const {
    switch (difficulty) {
    case 1: return "Easy";
    case 3: return "Hard";
    default: return "Normal";
    }
}
void Game::set_difficulty(int d) {
    if (d < 1) d = 1;
    if (d > 3) d = 3;
    difficulty = d;
}
int Game::get_wave() const {
    return wave;
}
const Player& Game::get_player() const {
    return *player;
}
const Bag& Game::get_bag() const {
    return bag;
}