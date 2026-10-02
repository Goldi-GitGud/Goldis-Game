#include "Player.h"
#include <iostream>
using namespace std;

Player::Player(int h, int ap, int a, int l, int exp, int g, int d, const Weapon& w)
    : health(h), attack_power(ap), armor(a), level(l), experience(exp), gold(g), dodge_chance(d), weapon(w) {
}

Player::Player() // this will be the default stats for default player class
    : health(100), attack_power(10), armor(5), level(1), experience(0), gold(0), dodge_chance(5), weapon(Weapon()) {
}

Player::set_health(int h){
    health =h;
}
Player::get_health() const{
    return health;
}
Player::set_bonus_health(int bh){
    bonus_health = bh;
}
Player::get_bonus_health() const{
    return bonus_health;
}
Player::get_attack_power() const{
    return attack_power;
}
Player::set_attack_power(int ap){
    attack_power = ap;
}
Player::get_bonus_attack_power() const{
    return bonus_attack_power;
}
Player::set_bonus_attack_power(int bap){
    bonus_attack_power = bap;
}
Player::get_armor() const{
    return armor;
}
Player::set_armor(int a){
    armor = a;
}
Player::get_bonus_armor() const{
    return bonus_armor;
}
Player::set_bonus_armor(int ba){
    bonus_armor = ba;
}
Player::get_dodge_chance() const{
    return dodge_chance;
}
Player::set_dodge_chance(int d){
    dodge_chance = d;
}
Player::get_bonus_dodge_chance() const{
    return bonus_dodge_chance;
}
Player::set_bonus_dodge_chance(int bd){
    bonus_dodge_chance = bd;
}
Player::get_level() const{
    return level;
}
Player::set_level(int l){
    level = l;
}
Player::get_experience() const{
    return experience;
}
Player::set_experience(int exp){
    experience = exp;
}
Player::get_gold() const{
    return gold;
}
Player::set_gold(int g){
    gold = g;
}
Player::get_weapon() const{
    return weapon;
}
Player::equip_weapon(const Weapon& w){
    weapon = w;
}
Player::display_stats() const {
    cout<< "____________________________________________" << endl;
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
    cout<< "____________________________________________" << endl;
}
Player::attack_enemy(Enemy& enemy) {
    if(roll_chance(enemy.get_dodge_chance())) {
        cout << "The enemy dodged your attack!" << endl;
        return;
    }
    int damage = attack_power + bonus_attack_power - enemy.get_armor();
    if (damage < 0) damage = 0;
    enemy.take_damage(damage);
    cout << "You attacked the enemy for " << damage << " damage!" << endl;
    if(enemy.get_health() <= 0) {
        cout << "You defeated the enemy!" << endl;
        gain_experience(enemy.get_experience_reward());
        gain_gold(enemy.get_gold_reward());
    }
}