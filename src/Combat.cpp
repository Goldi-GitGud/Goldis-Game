#include "Combat.h"
#include "Utils.h"
#include <iostream>
#include <string>
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

// Difficulty as a percent of base stats: easy 80%, normal 100%, hard 130%.
// Wave scaling multiplies on top: +8% HP and +6% attack per wave after wave 1,
// +1 armor every 4 waves, +1 dodge every 5 waves (capped). Scaling wave is
// clamped to 20 so numbers stay sane in endless runs.
static int difficulty_percent(int difficulty) {
    if (difficulty == 1) return 80;   // easy
    if (difficulty == 3) return 130;  // hard
    return 100;                       // normal
}

static int scaled_wave(int wave_number) {
    if (wave_number < 1) return 1;
    if (wave_number > 20) return 20;
    return wave_number;
}

// Short effect summary for the item menu, e.g. "Heals 20 HP, +10 Atk this wave".
static string item_effect_text(const Item& item) {
    string text;
    if (item.getHealthBonus() > 0)
        text += "Heals " + to_string(item.getHealthBonus()) + " HP";
    if (item.getAttackBonus() > 0) {
        if (!text.empty()) text += ", ";
        text += "+" + to_string(item.getAttackBonus()) + " Atk this wave";
    }
    if (item.getArmorBonus() > 0) {
        if (!text.empty()) text += ", ";
        text += "+" + to_string(item.getArmorBonus()) + " Armor this wave";
    }
    if (item.getDodgeBonus() > 0) {
        if (!text.empty()) text += ", ";
        text += "+" + to_string(item.getDodgeBonus()) + "% Dodge this wave";
    }
    if (text.empty()) text = "No effect";
    return text;
}

// Rolls item/weapon drops after a kill and puts them straight into the bag.
// Item chance grows with wave (31% -> 50%, +/-5 for hard/easy); weapons stay
// rare (5% -> 15%). Higher waves can drop bigger potions.
static void maybe_drop_loot(Bag& bag, const Enemy& enemy) {
    int wave = scaled_wave(enemy.get_wave_number());
    int item_chance = 30 + wave;
    if (item_chance > 50) item_chance = 50;
    if (enemy.get_difficulty() == 3) item_chance += 5;
    else if (enemy.get_difficulty() == 1) item_chance -= 5;

    if (roll_chance(item_chance)) {
        static const Item::ItemType small_potions[] = {
            Item::ItemType::Small_Health_potion, Item::ItemType::Small_Defense_potion,
            Item::ItemType::Small_Dodge_potion, Item::ItemType::Small_Attack_potion,
        };
        static const Item::ItemType medium_potions[] = {
            Item::ItemType::Medium_Health_potion, Item::ItemType::Medium_Defense_potion,
            Item::ItemType::Medium_Dodge_potion, Item::ItemType::Medium_Attack_potion,
        };
        static const Item::ItemType large_potions[] = {
            Item::ItemType::Large_Health_potion, Item::ItemType::Large_Defense_potion,
            Item::ItemType::Large_Dodge_potion, Item::ItemType::Large_Attack_potion,
        };
        Item::ItemType drop;
        if (wave <= 2) {
            drop = small_potions[random_int(0, 3)];
        }
        else if (wave <= 5) {
            drop = roll_chance(50) ? small_potions[random_int(0, 3)]
                                   : medium_potions[random_int(0, 3)];
        }
        else {
            int roll = random_int(1, 100);
            if (roll <= 30) drop = small_potions[random_int(0, 3)];
            else if (roll <= 70) drop = medium_potions[random_int(0, 3)];
            else drop = large_potions[random_int(0, 3)];
        }
        Item item(drop);
        bag.addItem(item);
        cout << "The " << enemy.type_to_string() << " dropped a " << item.get_name()
             << " (added to your bag)!" << endl;
    }

    int weapon_chance = 5 + wave / 2;
    if (weapon_chance > 15) weapon_chance = 15;
    if (roll_chance(weapon_chance)) {
        static const Weapon::WeaponType weapons[] = {
            Weapon::WeaponType::Sword, Weapon::WeaponType::Greatsword,
            Weapon::WeaponType::Dagger, Weapon::WeaponType::Spear,
            Weapon::WeaponType::Axe, Weapon::WeaponType::Mace,
            Weapon::WeaponType::Greataxe, Weapon::WeaponType::Katana,
            Weapon::WeaponType::Twinblade, Weapon::WeaponType::Saber,
        };
        Weapon weapon(weapons[random_int(0, 9)]);
        bag.addWeapon(weapon);
        cout << "The " << enemy.type_to_string() << " dropped a " << weapon.get_name()
             << " (added to your bag)!" << endl;
    }
}

Combat::Combat(Player& p, Bag& b) : player(p), bag(b), wave_active(false) {}

bool Combat::is_wave_active() const {
    return wave_active;
}

// ---- wave setup ----

void Combat::spawn_wave(int wave_number, int difficulty) {
    enemies.clear();

    // wave 1 -> 1 enemy, wave 2 -> 2, wave 4 -> 3 ... capped at 5
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

    // Wave+difficulty stat scaling (wave capped at 20, see scaled_wave).
    int wave = scaled_wave(wave_number);
    int diff = difficulty_percent(difficulty);
    int hp_percent = diff * (100 + 8 * (wave - 1)) / 100;
    int atk_percent = diff * (100 + 6 * (wave - 1)) / 100;
    int armor_bonus = (wave - 1) / 4 + (difficulty == 3 ? 1 : 0);
    int dodge_bonus = (wave - 1) / 5;
    if (dodge_bonus > 6) dodge_bonus = 6;
    for (int i = 0; i < count; i++) {
        int index = random_int(0, static_cast<int>(eligible.size()) - 1);
        Enemy enemy(eligible[index]);
        enemy.set_wave(wave_number, difficulty);
        enemy.apply_wave_scaling(hp_percent, atk_percent, armor_bonus, dodge_bonus);
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
    cout << "  Atk: " << player.get_attack_power() + player.get_bonus_attack_power()
         << " | Armor: " << player.get_armor() + player.get_bonus_armor()
         << " | Dodge: " << player.get_dodge_chance() + player.get_bonus_dodge_chance() << "%"
         << " | XP: " << player.get_experience() << " | Gold: " << player.get_gold()
         << " | Weapon: " << player.get_weapon().get_name() << endl;
    cout << "Enemies:" << endl;
    for (size_t i = 0; i < enemies.size(); i++) {
        const Enemy& enemy = enemies[i];
        cout << "  " << i + 1 << ". " << enemy.type_to_string() << "  HP: ";
        if (enemy.is_alive())
            cout << enemy.get_health() << "/" << enemy.get_max_health()
                 << " (Atk " << enemy.get_attack_power()
                 << ", Armor " << enemy.get_armor() << ")";
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
    // XP/gold already include wave+difficulty scaling via the enemy's stored wave.
    player.gain_experience(enemy.get_experience_reward());
    player.gain_gold(enemy.get_gold_reward());
    maybe_drop_loot(bag, enemy);
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
        cout << "  " << i + 1 << ". " << items[i].get_name()
             << " -- " << item_effect_text(items[i]) << endl;
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
    // Player bleed ticks once here, at the start of the enemies' turn (ignores
    // dodge). A lethal tick ends the round before any enemy swings.
    if (player.tick_bleed() > 0 && !player.is_alive()) {
        player.player_death();
        return;
    }
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
    player.clear_temporary_effects(); // potion boosts only last for one wave (bleed clears too)
    for (Enemy& enemy : enemies) enemy.clear_bleed(); // defensive: dead-wave bleed never leaks

    if (!player.is_alive()) {
        cout << "You were defeated on wave " << wave_number << "." << endl;
        return Result::PlayerDied;
    }
    cout << "Wave " << wave_number << " cleared!" << endl;
    return Result::Cleared;
}