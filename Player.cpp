#include "Player.h"
#include "Enemy.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

static const json& player_data() {
    static const json data = [] {
        ifstream file("Player_data.json");
        if (!file) throw runtime_error("Could not open Player_data.json");
        json j;
        file >> j;
        return j;
        }();
    return data;
}

// These two are free functions (not Player members) so the constructor can use them
// in its initializer list, before the Player object is fully built.
static string class_name(Player::PlayerClass pc) {
    switch (pc) {
    case Player::PlayerClass::Warrior: return "Warrior";
    case Player::PlayerClass::Knight: return "Knight";
    case Player::PlayerClass::Rogue: return "Rogue";
    case Player::PlayerClass::Paladin: return "Paladin";
    case Player::PlayerClass::Berserker: return "Berserker";
    case Player::PlayerClass::Monk: return "Monk";
    case Player::PlayerClass::Assassin: return "Assassin";
    case Player::PlayerClass::Duelist: return "Duelist";
    case Player::PlayerClass::Hunter: return "Hunter";
    default: return "Unknown";
    }
}
static const json& player_entry(Player::PlayerClass pc) {
    return player_data().at(class_name(pc));
}

// Weapon has no default constructor, so it has to be built in the initializer list
// (the part after the colon), before the constructor body runs.
Player::Player(PlayerClass pc)
    : type(pc),
      weapon(Weapon::string_to_type(player_entry(pc).at("WeaponType").get<string>())) {
    const json& p = player_entry(pc);
    health = p.at("Health");
    attack_power = p.at("AttackPower");
    armor = p.at("Armor");
    dodge_chance = p.at("DodgeChance");
    level = p.at("Level");
    gold = p.at("Gold");
    experience = 0;
    bonus_health = weapon.getHealthBonus();
    bonus_attack_power = weapon.getAttackBonus();
    bonus_armor = weapon.getArmorBonus();
    bonus_dodge_chance = weapon.getDodgeBonus();
}

Player::PlayerClass Player::getType() const {
    return type;
}
string Player::Type_to_string() const {
    return class_name(type);
}
int Player::get_health() const {
    return health;
}
int Player::get_bonus_health() const {
    return bonus_health;
}
int Player::get_attack_power() const {
    return attack_power;
}
int Player::get_bonus_attack_power() const {
    return bonus_attack_power;
}
int Player::get_armor() const {
    return armor;
}
int Player::get_bonus_armor() const {
    return bonus_armor;
}
int Player::get_dodge_chance() const {
    return dodge_chance;
}
int Player::get_bonus_dodge_chance() const {
    return bonus_dodge_chance;
}
int Player::get_level() const {
    return level;
}
int Player::get_experience() const {
    return experience;
}
int Player::get_gold() const {
    return gold;
}
const Weapon& Player::get_weapon() const {
    return weapon;
}
void Player::set_type(PlayerClass pc) {
    type = pc;
}
void Player::set_health(int h) {
    health = h;
}
void Player::set_bonus_health(int bh) {
    bonus_health = bh;
}
void Player::set_attack_power(int ap) {
    attack_power = ap;
}
void Player::set_bonus_attack_power(int bap) {
    bonus_attack_power = bap;
}
void Player::set_armor(int a) {
    armor = a;
}
void Player::set_bonus_armor(int ba) {
    bonus_armor = ba;
}
void Player::set_dodge_chance(int d) {
    dodge_chance = d;
}
void Player::set_bonus_dodge_chance(int bd) {
    bonus_dodge_chance = bd;
}
void Player::set_level(int l) {
    level = l;
}
void Player::set_experience(int exp) {
    experience = exp;
}
void Player::set_gold(int g) {
    gold = g;
}
void Player::equip_weapon(const Weapon& w) {
    weapon = w;
}
void Player::display_stats() const {
    cout << "____________________________________________" << endl;
    cout << "Player Stats (" << Type_to_string() << "):" << endl;
    cout << "Health: " << health << endl;
    cout << "Attack Power: " << attack_power << endl;
    cout << "Armor: " << armor << endl;
    cout << "Level: " << level << endl;
    cout << "Experience: " << experience << endl;
    cout << "Gold: " << gold << endl;
    cout << "Dodge Chance: " << dodge_chance << "%" << endl;
    cout << "Weapon Stats:" << endl;
    weapon.display_stats();
    cout << "____________________________________________" << endl;
}
void Player::attack_enemy(Enemy& enemy) {
    int damage = attack_power + bonus_attack_power - enemy.get_armor();
    if (damage < 0) damage = 0;
    enemy.take_damage(damage);
    cout << "You attacked the enemy for " << damage << " damage!" << endl;
}
void Player::take_damage(int damage) {
    if (dodge_attack()) {
        return;
    }
    int total_armor = armor + bonus_armor;
    int damage_taken = damage - total_armor;
    if (damage_taken < 0) damage_taken = 0;
    health -= damage_taken;
    cout << "You took " << damage_taken << " damage!" << endl;
    if (health <= 0) {
        player_death();
    }
}
void Player::gain_experience(int exp) {
    // this still needs to be adjusted and edge cases handled, but it works for now
    experience += exp;
    cout << "You gained " << exp << " experience!" << endl;
    while (experience >= level * 100) {
        gain_level();
    }
}
// Swaps the old weapon's bonuses for the new weapon's, so potion boosts on top of them are kept.
void Player::change_weapon(const Weapon& new_weapon) {
    bonus_health -= weapon.getHealthBonus();
    bonus_attack_power -= weapon.getAttackBonus();
    bonus_armor -= weapon.getArmorBonus();
    bonus_dodge_chance -= weapon.getDodgeBonus();
    weapon = new_weapon;
    cout << "You equipped a new weapon: " << weapon.get_name() << endl;
    bonus_health += weapon.getHealthBonus();
    bonus_attack_power += weapon.getAttackBonus();
    bonus_armor += weapon.getArmorBonus();
    bonus_dodge_chance += weapon.getDodgeBonus();
}
// Potions add to the bonus stats, so resetting them to just the weapon's bonuses removes the potion boosts.
void Player::clear_temporary_effects() {
    bonus_health = weapon.getHealthBonus();
    bonus_attack_power = weapon.getAttackBonus();
    bonus_armor = weapon.getArmorBonus();
    bonus_dodge_chance = weapon.getDodgeBonus();
}
void Player::gain_gold(int amount) {
    gold += amount;
    cout << "You gained " << amount << " gold!" << endl;
}
void Player::lose_gold(int amount) {
    gold -= amount;
    if (gold < 0) gold = 0;
    cout << "You lost " << amount << " gold!" << endl;
}
void Player::lose_experience(int amount) {
    experience -= amount;
    if (experience < 0) experience = 0;
    cout << "You lost " << amount << " experience!" << endl;
}
void Player::player_death() {
    cout << "You have died!" << endl;
    lose_experience(experience / 2);
    lose_gold(static_cast<int>(gold * 0.4));
}
void Player::gain_level() {
    // stats will be adjusted later
    level++;
    health += 20;
    attack_power += 5;
    armor += 2;
    dodge_chance += 1;
    cout << "You leveled up! You are now level " << level << "!" << endl;
}
void Player::use_item(const Item& item) {
    item.apply_effect(*this);
    cout << "You used " << item.get_name() << "!" << endl;
}
void Player::heal(int amount) {
    health += amount;
    cout << "You healed " << amount << " health!" << endl;
}
void Player::increase_attack_power(int amount) {
    attack_power += amount;
    cout << "Your attack power increased by " << amount << "!" << endl;
}
void Player::increase_armor(int amount) {
    armor += amount;
    cout << "Your armor increased by " << amount << "!" << endl;
}
void Player::increase_dodge_chance(int amount) {
    dodge_chance += amount;
    cout << "Your dodge chance increased by " << amount << "%!" << endl;
}
bool Player::dodge_attack() {
    if (roll_chance(dodge_chance + bonus_dodge_chance)) {
        cout << "You dodged the enemy's attack!" << endl;
        return true;
    }
    return false;
}
bool Player::is_alive() const {
    return health > 0;
}