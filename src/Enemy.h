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
    int max_health;   // HP cap: set from JSON, rescaled per wave; bounds regen/lifesteal healing
    int attack_power;
    int armor;
    int dodge_chance;
    int min_level_req;
    int wave_number;  // wave this enemy spawned on (1-based); scales XP/gold rewards
    int difficulty;   // 1 = easy, 2 = normal, 3 = hard; scales XP/gold rewards

public:
    explicit Enemy(EnemyType et);
    EnemyType get_type() const;
    std::string type_to_string() const;
    int get_health() const;
    int get_attack_power() const;
    int get_armor() const;
    int get_dodge_chance() const;
    int get_min_level_req() const;
    int get_max_health() const;
    int get_wave_number() const;
    int get_difficulty() const;
    int get_experience_reward() const;
    int get_gold_reward() const;
    void set_wave(int wave, int diff); // remembers spawn wave+difficulty for reward scaling
    void apply_wave_scaling(int hp_percent, int atk_percent, int armor_bonus, int dodge_bonus);
    void set_health(int h);
    void set_attack_power(int ap);
    void set_armor(int a);
    void set_dodge_chance(int d);
    void set_min_level_req(int l);
    void take_damage(int damage);
    void attack_player(Player& player);
    bool dodge_attack() const;
    bool is_alive() const;
};