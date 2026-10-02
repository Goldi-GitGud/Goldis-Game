#pragma once

class Enemy {
public:
    enum class EnemyType {
        Slime,
        Goblin,
        Zombie,
        Orc,
        Skeleton,
        Troll,
        cyclop,
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
    int level;

public:
    Enemy(int h, int ap, int a, int d, int l);
    Enemy();
    int get_health() const;
    int get_attack_power() const;
    int get_armor() const;
    int get_dodge_chance() const;
    int get_level() const;
    int get_experience_reward() const;
    int get_gold_reward() const;
    void set_health(int h);
    void set_attack_power(int ap);
    void set_armor(int a);
    void set_dodge_chance(int d);
    void set_level(int l);
    void take_damage(int damage);
    void attack_player(Player& player);
    EnemyType get_type() const;
};