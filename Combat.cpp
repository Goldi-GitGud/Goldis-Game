#include "Combat.h"
#include "Utils.h"
#include <iostream>
using namespace std;

// Every enemy that can appear in waves. Which ones actually spawn depends on the
// player's level: spawn_wave only keeps enemies whose MinLevel in Enemy_data.json is
// at or below it. Any type listed here must have an entry in Enemy_data.json,
// otherwise constructing that Enemy will throw.
static const vector<Enemy::EnemyType> spawn_pool = {
    Enemy::EnemyType::Slime,
    Enemy::EnemyType::Goblin,
    Enemy::EnemyType::Zombie,
    Enemy::EnemyType::Orc,
    Enemy::EnemyType::Skeleton,
    Enemy::EnemyType::Troll,
    Enemy::EnemyType::Cyclop,
    Enemy::EnemyType::Vampire,
    Enemy::EnemyType::Werewolf,
    Enemy::EnemyType::Lich,
    Enemy::EnemyType::Demon,
    Enemy::EnemyType::Dullahan,
    Enemy::EnemyType::Dragon,
};

// Placeholder: how strong enemies are on each difficulty, as a percentage of their JSON stats.
static int difficulty_percent(int difficulty) {
    if (difficulty == 1) return 80;   // easy
    if (difficulty == 3) return 130;  // hard
    return 100;                       // normal
}

Combat::Combat(Player& p, Bag& b) : player(p), bag(b), wave_active(false) {}

bool Combat::is_wave_active() const {
    return wave_active;
}

// ---- wave setup ----

void Combat::spawn_wave(int wave_number, int difficulty) {
    enemies.clear();

    // placeholder scaling: wave 1 -> 1 enemy, wave 2 -> 2, wave 4 -> 3 ... capped at 5
    int count = 1 + wave_number / 2;
    if (count > 5) count = 5;

    // only enemies the player is high enough level for
    vector<Enemy::EnemyType> eligible;
    for (Enemy::EnemyType type : spawn_pool) {
        if (Enemy(type).get_min_level_req() <= player.get_level()) {
            eligible.push_back(type);
        }
    }
    if (eligible.empty()) {
        eligible.push_back(spawn_pool.front());
    }

    int percent = difficulty_percent(difficulty);
    for (int i = 0; i < count; i++) {
        int index = random_int(0, static_cast<int>(eligible.size()) - 1);
        Enemy enemy(eligible[index]);
        enemy.set_health(enemy.get_health() * percent / 100);
        enemy.set_attack_power(enemy.get_attack_power() * percent / 100);
        enemies.push_back(enemy);
    }
}

// ---- helpers ----

bool Combat::any_enemy_alive() const {
    return alive_enemy_count() > 0;
}

int Combat::alive_enemy_count() const {
    int count = 0;
    for (const Enemy& enemy : enemies) {
        if (enemy.is_alive()) count++;
    }
    return count;
}

void Combat::show_status() const {
    cout << "____________________________________________" << endl;
    cout << "You (" << player.Type_to_string() << ", level " << player.get_level() << ")  HP: "
         << player.get_health() << endl;
    cout << "Enemies:" << endl;
    for (size_t i = 0; i < enemies.size(); i++) {
        const Enemy& enemy = enemies[i];
        cout << "  " << i + 1 << ". " << enemy.type_to_string() << "  HP: ";
        if (enemy.is_alive()) cout << enemy.get_health();
        else cout << "defeated";
        cout << endl;
    }
    cout << "____________________________________________" << endl;
}

int Combat::pick_target() const {
    // only one enemy left: no need to ask
    if (alive_enemy_count() == 1) {
        for (size_t i = 0; i < enemies.size(); i++) {
            if (enemies[i].is_alive()) return static_cast<int>(i);
        }
    }
    cout << "Choose a target: ";
    while (true) {
        int choice = read_choice(1, static_cast<int>(enemies.size()));
        if (enemies[choice - 1].is_alive()) return choice - 1;
        cout << "That enemy is already defeated. Choose another: ";
    }
}

void Combat::reward_for(const Enemy& enemy) {
    player.gain_experience(enemy.get_experience_reward());
    player.gain_gold(enemy.get_gold_reward());
}

// ---- items ----

bool Combat::use_item_menu() {
    const vector<Item>& items = bag.getItems();
    if (items.empty()) {
        cout << "Your bag has no items." << endl;
        return false;
    }
    cout << "Items:" << endl;
    for (size_t i = 0; i < items.size(); i++) {
        cout << "  " << i + 1 << ". " << items[i].get_name() << endl;
    }
    cout << "  0. Back" << endl;
    cout << "Choose an item: ";
    int choice = read_choice(0, static_cast<int>(items.size()));
    if (choice == 0) return false;

    // Copy the item first: removing it from the bag would invalidate a reference into the bag.
    Item chosen = items[choice - 1];
    player.use_item(chosen);
    bag.removeItem(chosen);
    return true;
}

// ---- turns ----

Combat::TurnResult Combat::player_turn() {
    while (true) {
        cout << "1. Attack" << endl;
        cout << "2. Use item" << endl;
        cout << "3. View my stats" << endl;
        cout << "4. Pause" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 4);

        if (choice == 1) {
            int target = pick_target();
            player.attack_enemy(enemies[target]);
            if (!enemies[target].is_alive()) {
                reward_for(enemies[target]);
            }
            return TurnResult::Acted; // attacking ends the turn
        }
        if (choice == 2) {
            if (use_item_menu()) {
                return TurnResult::Acted; // using an item ends the turn too
            }
            // backed out or bag empty: the turn isn't used, ask again
        }
        else if (choice == 3) {
            player.display_stats(); // viewing stats is free, so ask again
        }
        else {
            return TurnResult::Paused;
        }
    }
}

void Combat::enemies_turn() {
    for (Enemy& enemy : enemies) {
        if (!player.is_alive()) return;
        if (enemy.is_alive()) {
            enemy.attack_player(player);
        }
    }
}

// ---- main loop ----

Combat::Result Combat::fight(int wave_number, int difficulty) {
    if (!wave_active) {
        spawn_wave(wave_number, difficulty);
        wave_active = true;
        cout << endl << "=== Wave " << wave_number << " ===" << endl;
    }
    else {
        cout << endl << "=== Wave " << wave_number << " (continued) ===" << endl;
    }

    while (player.is_alive() && any_enemy_alive()) {
        show_status();
        if (player_turn() == TurnResult::Paused) {
            return Result::Paused; // enemies stay as they are until the player comes back
        }
        if (any_enemy_alive()) {
            enemies_turn();
        }
    }

    wave_active = false;
    player.clear_temporary_effects(); // potion boosts only last for one wave

    if (!player.is_alive()) {
        cout << "You were defeated on wave " << wave_number << "." << endl;
        return Result::PlayerDied;
    }
    cout << "Wave " << wave_number << " cleared!" << endl;
    return Result::Cleared;
}