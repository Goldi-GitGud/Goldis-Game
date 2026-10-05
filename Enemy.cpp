#include "Enemy.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

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
Enemy::Enemy(EnemyType et) : type(et) {
    const json& e = enemy_data().at(type_to_string());
    health = e.at("Hp");
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
int Enemy::get_experience_reward() const {
    return min_level_req * 10; // Example calculation will be adjusted based on type and level in the future
}
int Enemy::get_gold_reward() const {
    return min_level_req * 5; // same as above
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
    player.take_damage(attack_power);
    cout << "The enemy attacked you for " << attack_power << " damage!" << endl;
}
bool Enemy::dodge_attack() const {
    return roll_chance(dodge_chance);
}
bool Enemy::is_alive() const {
    return health > 0;
}