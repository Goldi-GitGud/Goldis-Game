#include "Player.h"
#include "Enemy.h"
#include "Utils.h"
#include <iostream>
using namespace std;

Player::Player(int h, int ap, int a, int l, int exp, int g, int d, const Weapon& w)
    : health(h), attack_power(ap), armor(a), level(l), experience(exp), gold(g), dodge_chance(d), weapon(w) {
}

Player::Player() // this will be the default stats for default player class
    : health(100), attack_power(10), armor(5), level(1), experience(0), gold(0), dodge_chance(5), weapon(Weapon()) {
}

void Player::set_health(int h) {
    health = h;
}
int Player::get_health() const {
    return health;
}
void Player::set_bonus_health(int bh) {
    bonus_health = bh;
}
int Player::get_bonus_health() const {
    return bonus_health;
}
int Player::get_attack_power() const {
    return attack_power;
}
void Player::set_attack_power(int ap) {
    attack_power = ap;
}
int Player::get_bonus_attack_power() const {
    return bonus_attack_power;
}
void Player::set_bonus_attack_power(int bap) {
    bonus_attack_power = bap;
}
int Player::get_armor() const {
    return armor;
}
void Player::set_armor(int a) {
    armor = a;
}
int Player::get_bonus_armor() const {
    return bonus_armor;
}
void Player::set_bonus_armor(int ba) {
    bonus_armor = ba;
}
int Player::get_dodge_chance() const {
    return dodge_chance;
}
void Player::set_dodge_chance(int d) {
    dodge_chance = d;
}
int Player::get_bonus_dodge_chance() const {
    return bonus_dodge_chance;
}
void Player::set_bonus_dodge_chance(int bd) {
    bonus_dodge_chance = bd;
}
int Player::get_level() const {
    return level;
}
void Player::set_level(int l) {
    level = l;
}
int Player::get_experience() const {
    return experience;
}
void Player::set_experience(int exp) {
    experience = exp;
}
int Player::get_gold() const {
    return gold;
}
void Player::set_gold(int g) {
    gold = g;
}
const Weapon& Player::get_weapon() const {
    return weapon;
}
void Player::equip_weapon(const Weapon& w) {
    weapon = w;
}
void Player::display_stats() const {
    cout << "____________________________________________" << endl;
    cout << "Player Stats:" << endl;
    cout << "Health: " << health << endl;
    cout << "Attack Power: " << attack_power << endl;
    cout << "Armor: " << armor << endl;
    cout << "Level: " << level << endl;
    cout << "Experience: " << experience << endl;
    cout << "Gold: " << gold << endl;
    cout << "Dodge Chance: " << dodge_chance << "%" << endl;
    cout << "Equipped Weapon: " << weapon.get_name() << endl;
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
    experience += exp;
    cout << "You gained " << exp << " experience!" << endl;
    if (experience >= level * 100) {
        gain_level();
    }
}
bool Player::dodge_attack() {
    if (roll_chance(dodge_chance + bonus_dodge_chance)) {
        cout << "You dodged the enemy's attack!" << endl;
        return true;
    }
    return false;
}
void Player::change_weapon(const Weapon& new_weapon) {
    weapon = new_weapon;
    cout << "You equipped a new weapon: " << weapon.get_name() << endl;
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
    lose_gold((double)gold * 0.4);
}
void Player::gain_level() {
    level++;
    experience = 0;
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