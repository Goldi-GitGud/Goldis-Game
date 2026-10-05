#include "Combat.h"
#include "Utils.h"
#include <iostream>
#include <limits>
#include <stdexcept>
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

// Reads a whole number between min and max, asking again on bad input.
static int read_choice(int min, int max) {
    while (true) {
        int choice;
        if (cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        if (cin.eof()) {
            throw runtime_error("Input closed");
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a number from " << min << " to " << max << ": ";
    }
}

Combat::Combat(Player& p) : player(p) {}

// ---- wave setup ----

void Combat::spawn_wave(int wave_number) {
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

    for (int i = 0; i < count; i++) {
        int index = random_int(0, static_cast<int>(eligible.size()) - 1);
        enemies.push_back(Enemy(eligible[index]));
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

// ---- turns ----

void Combat::player_turn() {
    while (true) {
        cout << "1. Attack" << endl;
        cout << "2. View my stats" << endl;
        cout << "Enter your choice: ";
        int choice = read_choice(1, 2);

        if (choice == 1) {
            int target = pick_target();
            player.attack_enemy(enemies[target]);
            if (!enemies[target].is_alive()) {
                reward_for(enemies[target]);
            }
            return; // attacking ends the turn
        }
        player.display_stats(); // viewing stats is free, so ask again
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

bool Combat::run_wave(int wave_number) {
    spawn_wave(wave_number);
    cout << endl << "=== Wave " << wave_number << " ===" << endl;

    while (player.is_alive() && any_enemy_alive()) {
        show_status();
        player_turn();
        if (any_enemy_alive()) {
            enemies_turn();
        }
    }

    player.clear_temporary_effects(); // potion boosts only last for one wave

    if (!player.is_alive()) {
        cout << "You were defeated on wave " << wave_number << "." << endl;
        return false;
    }
    cout << "Wave " << wave_number << " cleared!" << endl;
    return true;
}