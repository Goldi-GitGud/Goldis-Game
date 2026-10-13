#pragma once
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Bag.h"

class Combat {
public:
    // What happened when fight() returned
    enum class Result { Cleared, PlayerDied, Paused };
private:
    enum class TurnResult { Acted, Paused };

    Player& player;
    Bag& bag;
    std::vector<Enemy> enemies;
    bool wave_active; // true while a wave is unfinished (e.g. paused halfway), so resuming doesn't spawn new enemies

    void spawn_wave(int wave_number, int difficulty);
    void show_status() const;
    bool any_enemy_alive() const;
    int alive_enemy_count() const;
    int pick_target() const;      // returns an index into enemies
    TurnResult player_turn();
    void enemies_turn();
    void reward_for(const Enemy& enemy);

public:
    Combat(Player& p, Bag& b);
    bool is_wave_active() const;
    // Lets the player pick an item from the bag and use it. Returns true if one was used.
    bool use_item_menu();
    // Plays (or continues) one wave. difficulty: 1 = easy, 2 = normal, 3 = hard.
    Result fight(int wave_number, int difficulty);
};