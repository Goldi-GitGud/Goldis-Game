#include "Enemy.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

static const json& enemy_data() {
    static const json data = [] {
        ifstream file("Enemy_data.json");
        if (!file) throw runtime_error("Could not open Enemy_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}
Enemy::Enemy(EnemyType et) : type(et), wave_number(1), difficulty(2) {
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
void Enemy::take_damage(int damage) {
    if (dodge_attack()) {
        cout << "The enemy dodged your attack!" << endl;
        return;
    }
    health -= damage;
    if (health <= 0) {
        cout << "You defeated a " << type_to_string() << "!" << endl;
    }
}
void Enemy::attack_player(Player& player) {
    if (!is_alive() || !player.is_alive()) return;
    // Troll special: regenerates a little at the start of each of its turns.
    if (type == EnemyType::Troll && health < max_health) {
        int regen = max_health * 5 / 100;
        if (regen < 3) regen = 3;
        health += regen;
        if (health > max_health) health = max_health;
        cout << "The Troll regenerates " << regen << " HP!" << endl;
    }
    int damage = attack_power;
    // Werewolf special: 20% chance to crit for 1.5x damage.
    bool crit = (type == EnemyType::Werewolf && roll_chance(20));
    if (crit) damage = damage * 3 / 2;
    int health_before = player.get_health();
    player.take_damage(damage);
    int dealt = health_before - player.get_health();
    if (dealt < 0) dealt = 0;
    if (crit) cout << "Critical hit! ";
    cout << "The " << type_to_string() << " attacked you for " << damage << " damage!" << endl;
    // Vampire special: drains 30% of the damage actually dealt as healing.
    if (type == EnemyType::Vampire && dealt > 0 && is_alive()) {
        int drain = dealt * 30 / 100;
        if (drain < 1) drain = 1;
        health += drain;
        if (health > max_health) health = max_health;
        cout << "The Vampire drains " << drain << " HP from you!" << endl;
    }
}
bool Enemy::dodge_attack() const {
    return roll_chance(dodge_chance);
}
bool Enemy::is_alive() const {
    return health > 0;
}