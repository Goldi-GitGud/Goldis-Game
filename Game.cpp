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
        // No max-HP exists yet (T4 adds the cap); show current HP plus the
        // weapon bonus, plus everything else the camp hub needs (T6).
        // XP threshold level*100 mirrors Player::gain_experience.
        cout << "____________________________________________" << endl;
        cout << "Camp - next up: wave " << wave << endl;
        cout << player->Type_to_string() << " Lv " << player->get_level()
             << " | HP: " << player->get_health() << " (bonus +" << player->get_bonus_health() << ")"
             << " | XP: " << player->get_experience() << "/" << player->get_level() * 100
             << " | Gold: " << player->get_gold()
             << " | Weapon: " << player->get_weapon().get_name() << endl;
        cout << "1. Start wave" << endl;
        cout << "2. Use item" << endl;
        cout << "3. View my stats" << endl;
        cout << "4. View bag" << endl;
        cout << "5. Heal (10g -> +20 HP, free rest +5 HP if broke)" << endl;
        cout << "6. Equip weapon" << endl;
        cout << "7. Shop (buy/sell)" << endl;
        cout << "8. Pause" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 8);
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
        case 5:
            camp_heal();
            break;
        case 6:
            camp_equip();
            break;
        case 7:
            camp_shop();
            break;
        default:
            return false;
        }
    }
}

void Game::camp_heal() {
    if (player->get_gold() >= 10) {
        player->lose_gold(10);
        player->heal(20);
    } else {
        cout << "Not enough gold, you rest a little instead." << endl;
        player->heal(5);
    }
}

void Game::camp_equip() {
    const vector<Weapon>& weapons = bag.getWeapons();
    if (weapons.empty()) {
        cout << "Your bag holds no spare weapons." << endl;
        return;
    }
    cout << "Equipped: " << player->get_weapon().get_name() << endl;
    for (size_t i = 0; i < weapons.size(); i++) {
        cout << i + 1 << ". " << weapons[i].get_name() << endl;
    }
    cout << "0. Back" << endl;
    cout << "Enter your choice: ";
    int choice = read_choice(0, static_cast<int>(weapons.size()));
    if (choice == 0) {
        return;
    }
    Weapon picked = weapons[static_cast<size_t>(choice) - 1];
    bag.removeWeapon(picked);
    bag.addWeapon(player->get_weapon()); // old weapon goes back to the bag
    player->change_weapon(picked);
}

// Minimal gold sink (DESIGN.md #4): fixed prices, potions + one weapon for
// sale, flat resale. T3/economy work can retune prices without changing saves.
void Game::camp_shop() {
    while (true) {
        cout << "____________________________________________" << endl;
        cout << "Shop (your gold: " << player->get_gold() << ")" << endl;
        cout << "1. Buy Small_Health_potion (15g)" << endl;
        cout << "2. Buy Small_Attack_potion (15g)" << endl;
        cout << "3. Buy Small_Defense_potion (10g)" << endl;
        cout << "4. Buy Dagger (40g)" << endl;
        cout << "5. Sell an item (5g each)" << endl;
        cout << "6. Sell a weapon (15g each)" << endl;
        cout << "7. Leave shop" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 7);
        if (choice == 7) {
            return;
        }
        if (choice >= 1 && choice <= 4) {
            Item::ItemType itype = Item::ItemType::Small_Health_potion;
            bool is_weapon = (choice == 4);
            int price = 15;
            if (choice == 2) { itype = Item::ItemType::Small_Attack_potion; price = 15; }
            if (choice == 3) { itype = Item::ItemType::Small_Defense_potion; price = 10; }
            if (choice == 4) { price = 40; }
            if (player->get_gold() < price) {
                cout << "Not enough gold." << endl;
                continue;
            }
            player->lose_gold(price);
            if (is_weapon) {
                bag.addWeapon(Weapon(Weapon::WeaponType::Dagger));
                cout << "Bought a Dagger. Equip it from the camp menu." << endl;
            } else {
                bag.addItem(Item(itype));
                cout << "Bought a potion." << endl;
            }
        } else if (choice == 5) {
            const vector<Item>& items = bag.getItems();
            if (items.empty()) {
                cout << "No items to sell." << endl;
                continue;
            }
            for (size_t i = 0; i < items.size(); i++) {
                cout << i + 1 << ". " << items[i].get_name() << endl;
            }
            cout << "0. Back" << endl;
            cout << "Enter your choice: ";
            int pick = read_choice(0, static_cast<int>(items.size()));
            if (pick == 0) {
                continue;
            }
            Item sold = items[static_cast<size_t>(pick) - 1];
            bag.removeItem(sold);
            player->gain_gold(5);
        } else {
            const vector<Weapon>& weapons = bag.getWeapons();
            if (weapons.empty()) {
                cout << "No weapons to sell." << endl;
                continue;
            }
            for (size_t i = 0; i < weapons.size(); i++) {
                cout << i + 1 << ". " << weapons[i].get_name() << endl;
            }
            cout << "0. Back" << endl;
            cout << "Enter your choice: ";
            int pick = read_choice(0, static_cast<int>(weapons.size()));
            if (pick == 0) {
                continue;
            }
            Weapon sold = weapons[static_cast<size_t>(pick) - 1];
            bag.removeWeapon(sold);
            player->gain_gold(15);
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
bool Game::is_mid_wave() const {
    return combat.has_value() && combat->is_wave_active();
}
const Player& Game::get_player() const {
    return *player;
}
const Bag& Game::get_bag() const {
    return bag;
}