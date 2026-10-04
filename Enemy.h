#pragma once
#include <string>

class Player; // Forward declaration of Player class

class Enemy {
public:
    enum class EnemyType {
        // will sort them by level for randomizer
        Slime,
        Goblin,
        Zombie,
        Orc,
        Skeleton,
        Troll,
        Cyclop,
        Dragon,
        Lich,
        Vampire,
        Werewolf,
        Demon,
        Dullahan,
    };
private:
    EnemyType type;
    int health;
    int attack_power;
    int armor;
    int dodge_chance;
    int min_level_req;

public:
    Enemy(EnemyType et);
    EnemyType get_type() const;
    std::string type_to_string() const;
    int get_health() const;
    int get_attack_power() const;
    int get_armor() const;
    int get_dodge_chance() const;
    int get_min_level_req() const;
    int get_experience_reward() const;
    int get_gold_reward() const;
    void set_health(int h);
    void set_attack_power(int ap);
    void set_armor(int a);
    void set_dodge_chance(int d);
    void set_min_level_req(int l);
    void take_damage(int damage);
    void attack_player(Player& player);
    bool dodge_attack() const;
};