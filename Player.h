#pragma once

#include <string>
#include "Weapon.h"
#include "Item.h"
class Enemy; // Forward declaration of Enemy class

class Player {
public:
    enum class PlayerClass {
        Warrior,
        Knight,
        Rogue,
        Paladin,
        Berserker,
        Monk,
        Assassin,
        Duelist,
        Hunter
    };
private:
    PlayerClass type;
    int health;
    int bonus_health;
    int attack_power;
    int bonus_attack_power;
    int armor;
    int bonus_armor;
    int dodge_chance;
    int bonus_dodge_chance;
    int level;
    int experience;
    int gold;
    Weapon weapon;

public:
    explicit Player(PlayerClass pc);
    PlayerClass getType() const;
    std::string Type_to_string() const;
    int get_health() const;
    int get_bonus_health() const;
    int get_attack_power() const;
    int get_bonus_attack_power() const;
    int get_armor() const;
    int get_bonus_armor() const;
    int get_dodge_chance() const;
    int get_bonus_dodge_chance() const;
    int get_level() const;
    int get_experience() const;
    int get_gold() const;
    const Weapon& get_weapon() const;
    void set_type(PlayerClass pc);
    void set_health(int h);
    void set_bonus_health(int bh);
    void set_attack_power(int ap);
    void set_bonus_attack_power(int bap);
    void set_armor(int a);
    void set_bonus_armor(int ba);
    void set_dodge_chance(int d);
    void set_bonus_dodge_chance(int bd);
    void set_level(int l);
    void set_experience(int exp);
    void set_gold(int g);
    void equip_weapon(const Weapon& w);
    void display_stats() const;

    void attack_enemy(Enemy& enemy);
    void take_damage(int damage);
    void gain_experience(int exp);
    void change_weapon(const Weapon& new_weapon);
    void clear_temporary_effects(); // potion boosts wear off; only the weapon's bonuses stay
    void gain_gold(int amount);
    void lose_gold(int amount);
    void lose_experience(int amount);
    void player_death();
    void gain_level();
    void use_item(const Item& item);
    void heal(int amount);
    void increase_attack_power(int amount);
    void increase_armor(int amount);
    void increase_dodge_chance(int amount);
    bool dodge_attack();
    bool is_alive() const;
};