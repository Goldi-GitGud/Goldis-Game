#include "Enemy.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;


Enemy::Enemy(EnemyType et)
: type(et) {
        std::ifstream file("Enemy_data.json");
        if (!file){
            throw std::runtime_error("Could not open Enemy_data.json");
        }
        json data;
        file>>data;
        health=data[type_to_string()]["Hp"];
        attack_power=data[type_to_string()]["Attack"];
        armor=data[type_to_string()]["Armor"];
        dodge_chance=data[type_to_string()]["Dodge"];
        min_level_req=data[type_to_string()]["MinLevel"];
}
Enemy::EnemyType Enemy::get_type() const {
    return type;
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
string Enemy::type_to_string() const {
    switch (type) {
        case EnemyType::Slime: return "Slime";
        case EnemyType::Goblin: return "Goblin";
        case EnemyType::Zombie: return "Zombie";
        case EnemyType::Orc: return "Orc";
        case EnemyType::Skeleton: return "Skeleton";
        case EnemyType::Troll: return "Troll";
        case EnemyType::cyclop: return "cyclop";
        case EnemyType::Dragon: return "Dragon";
        case EnemyType::Lich: return "Lich";
        case EnemyType::Vampire: return "Vampire";
        case EnemyType::Werewolf: return "Werewolf";
        case EnemyType::Demon: return "Demon";
        case EnemyType::Dullahan: return "Dullahan";
        default: return "Unknown";
    }
}