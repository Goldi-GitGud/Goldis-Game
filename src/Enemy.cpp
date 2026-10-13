#include "Enemy.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Affix tuning shared with the player side (see Player.cpp).
static constexpr int BLEED_CHANCE = 25;        // % chance a Goblin hit adds a bleed stack
static constexpr int BLEED_TICK_PER_STACK = 3; // HP lost per stack at turn start (ignores dodge/armor)
static constexpr int BLEED_MAX_STACKS = 5;
static constexpr int CRIT_CHANCE = 20;         // % chance to crit (Werewolf + Dullahan)

static const json& enemy_data() {
    static const json data = [] {
        ifstream file("data/Enemy_data.json");
        if (!file) throw runtime_error("Could not open data/Enemy_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}
Enemy::Enemy(EnemyType et) : type(et), wave_number(1), difficulty(2), bleed_stacks(0) {
    const json& e = enemy_data().at(type_to_string());
    health = e.at("Hp");
    max_health = health;
    attack_power = e.at("Attack");
    armor = e.at("Armor");
    dodge_chance = e.at("Dodge");
    min_level_req = e.at("MinLevel");
}
Enemy::EnemyType Enemy::get_type() const {
    return type;
}
string Enemy::type_to_string() const {
    switch (type) {
    case EnemyType::Slime: return "Slime";
    case EnemyType::Goblin: return "Goblin";
    case EnemyType::Zombie: return "Zombie";
    case EnemyType::Orc: return "Orc";
    case EnemyType::Skeleton: return "Skeleton";
    case EnemyType::Troll: return "Troll";
    case EnemyType::Cyclop: return "Cyclop";
    case EnemyType::Dragon: return "Dragon";
    case EnemyType::Lich: return "Lich";
    case EnemyType::Vampire: return "Vampire";
    case EnemyType::Werewolf: return "Werewolf";
    case EnemyType::Demon: return "Demon";
    case EnemyType::Dullahan: return "Dullahan";
    default: return "Unknown";
    }
}
int Enemy::get_health() const {
    return health;
}
int Enemy::get_attack_power() const {
    return attack_power;
}
int Enemy::get_armor() const {
    return armor;
}
int Enemy::get_dodge_chance() const {
    return dodge_chance;
}
int Enemy::get_min_level_req() const {
    return min_level_req;
}
int Enemy::get_max_health() const {
    return max_health;
}
int Enemy::get_wave_number() const {
    return wave_number;
}
int Enemy::get_difficulty() const {
    return difficulty;
}
// Base XP/gold grow with the enemy's level; the wave it spawned on and the
// difficulty add a bonus on top so later/harder waves stay rewarding.
int Enemy::get_experience_reward() const {
    int w = wave_number;
    if (w < 1) w = 1;
    if (w > 20) w = 20; // scaling caps at wave 20
    int diff_percent = 100;
    if (difficulty == 1) diff_percent = 80;
    else if (difficulty == 3) diff_percent = 130;
    return (min_level_req * 10 + w * 2) * diff_percent / 100;
}
int Enemy::get_gold_reward() const {
    int w = wave_number;
    if (w < 1) w = 1;
    if (w > 20) w = 20; // scaling caps at wave 20
    int diff_percent = 100;
    if (difficulty == 1) diff_percent = 80;
    else if (difficulty == 3) diff_percent = 130;
    return (min_level_req * 5 + w) * diff_percent / 100;
}
void Enemy::set_wave(int wave, int diff) {
    wave_number = wave < 1 ? 1 : wave;
    difficulty = (diff >= 1 && diff <= 3) ? diff : 2;
}
// Applies wave+difficulty scaling in one place so health and max_health stay
// in sync. Percents combine difficulty and wave multipliers; bonuses are flat.
void Enemy::apply_wave_scaling(int hp_percent, int atk_percent, int armor_bonus, int dodge_bonus) {
    health = health * hp_percent / 100;
    if (health < 1) health = 1;
    max_health = health;
    attack_power = attack_power * atk_percent / 100;
    if (attack_power < 1) attack_power = 1;
    armor += armor_bonus;
    if (armor < 0) armor = 0;
    dodge_chance += dodge_bonus;
    if (dodge_chance < 0) dodge_chance = 0;
    if (dodge_chance > 50) dodge_chance = 50; // keep hits possible even at wave 20
}
void Enemy::set_health(int h) {
    health = h;
}
void Enemy::set_attack_power(int ap) {
    attack_power = ap;
}
void Enemy::set_armor(int a) {
    armor = a;
}
void Enemy::set_dodge_chance(int d) {
    dodge_chance = d;
}
void Enemy::set_min_level_req(int l) {
    min_level_req = l;
}
bool Enemy::take_damage(int damage) {
    if (dodge_attack()) {
        cout << "The " << type_to_string() << " dodges your attack!" << endl;
        return false;
    }
    health -= damage;
    if (health <= 0) {
        cout << "You defeated the " << type_to_string() << "!" << endl;
    }
    return true;
}
int Enemy::get_bleed_stacks() const {
    return bleed_stacks;
}
void Enemy::add_bleed_stack() {
    if (bleed_stacks < BLEED_MAX_STACKS) bleed_stacks++;
    cout << "The " << type_to_string() << " is bleeding (" << bleed_stacks
         << (bleed_stacks == 1 ? " stack" : " stacks") << ")!" << endl;
}
int Enemy::tick_bleed() {
    if (bleed_stacks <= 0 || !is_alive()) return 0;
    int tick = bleed_stacks * BLEED_TICK_PER_STACK;
    health -= tick;
    cout << "The " << type_to_string() << " bleeds for " << tick << " (" << bleed_stacks
         << (bleed_stacks == 1 ? " stack" : " stacks") << ")!" << endl;
    if (health <= 0) {
        cout << "You defeated the " << type_to_string() << "!" << endl;
    }
    return tick;
}
void Enemy::clear_bleed() {
    bleed_stacks = 0;
}
void Enemy::attack_player(Player& player) {
    if (!is_alive() || !player.is_alive()) return;
    // Bleed ticks at the start of our turn, ignoring dodge (and armor). A lethal
    // tick ends the turn: a bleeding-out enemy doesn't get to swing.
    if (tick_bleed() > 0 && !is_alive()) return;
    // Troll special: regenerates a little at the start of each of its turns.
    if (type == EnemyType::Troll && health < max_health) {
        int regen = max_health * 5 / 100;
        if (regen < 3) regen = 3;
        health += regen;
        if (health > max_health) health = max_health;
        cout << "The Troll regenerates " << regen << " HP!" << endl;
    }
    // Skeleton special: two sequential swings (each with its own dodge + armor
    // roll). The second still rolls after a dodge; wasted only if the target died.
    int swings = (type == EnemyType::Skeleton) ? 2 : 1;
    for (int s = 0; s < swings && player.is_alive(); s++) {
        int damage = attack_power;
        // Werewolf + Dullahan special: 20% chance to crit for 1.5x damage.
        bool crit = ((type == EnemyType::Werewolf || type == EnemyType::Dullahan)
                     && roll_chance(CRIT_CHANCE));
        if (crit) damage = damage * 3 / 2;
        int health_before = player.get_health();
        if (!player.take_damage(damage)) {
            cout << "You dodge the " << type_to_string() << "'s attack!" << endl;
            continue;
        }
        int dealt = health_before - player.get_health();
        if (dealt < 0) dealt = 0;
        int blocked = damage - dealt;
        if (blocked < 0) blocked = 0;
        if (crit) cout << "Critical! ";
        cout << "The " << type_to_string() << " hits you for " << dealt;
        if (blocked > 0) cout << " (" << blocked << " blocked)";
        cout << "." << endl;
        // Vampire special: drains 30% of the damage actually dealt as healing.
        if (type == EnemyType::Vampire && dealt > 0 && is_alive()) {
            int drain = dealt * 30 / 100;
            if (drain < 1) drain = 1;
            health += drain;
            if (health > max_health) health = max_health;
            cout << "The Vampire drains " << drain << " HP from you!" << endl;
        }
        // Goblin special: 25% chance on a landed hit to add 1 bleed stack (cap 5).
        if (type == EnemyType::Goblin && player.is_alive() && roll_chance(BLEED_CHANCE)) {
            player.add_bleed_stack();
        }
    }
}
bool Enemy::dodge_attack() const {
    return roll_chance(dodge_chance);
}
bool Enemy::is_alive() const {
    return health > 0;
}