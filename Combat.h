#pragma once
#include <vector>
#include "Player.h"
#include "Enemy.h"

class Combat {
private:
    Player& player;
    std::vector<Enemy> enemies;

    int alive_enemy_count() const;
    int pick_target() const;
    void show_status() const;
    void player_turn();
    void enemies_turn();
    void spawn_wave(int wave_number);
    void reward_for(const Enemy& enemy);
    bool any_enemy_alive() const;

public:
    explicit Combat(Player& p);
    bool run_wave(int wave_number);
};