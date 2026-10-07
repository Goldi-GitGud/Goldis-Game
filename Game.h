#pragma once
#include <optional>
#include <string>
#include "Player.h"
#include "Bag.h"
#include "Combat.h"

// Game owns everything that makes up one playthrough (player, bag, combat, current wave)
// and runs the wave loop. Menu decides *when* to start/pause it; Manager saves/loads its state.
class Game {
public:
    // What happened when run() returned
    enum class Result { Paused, GameOver };
private:
    // std::optional because Player has no default constructor: it starts empty and gets
    // filled with player.emplace(...) once a class is chosen or a save is loaded.
    std::optional<Player> player;
    Bag bag;
    std::optional<Combat> combat; // keeps references to player and bag, so it's declared after them
    int wave;
    int difficulty; // 1 = easy, 2 = normal, 3 = hard. A global setting, not part of a save.

    bool camp(); // the menu between waves. Returns false if the player chose to pause.

public:
    Game();
    // Combat holds references into this object, so copying a Game would leave them pointing at the original.
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void new_game(Player::PlayerClass pc);
    void load_state(const Player& p, const Bag& b, int wave_number); // used by Manager when loading
    bool has_game() const;
    // Plays from the current wave until the player pauses or dies.
    Result run();

    int get_difficulty() const;
    std::string get_difficulty_name() const;
    void set_difficulty(int d);
    int get_wave() const;
    const Player& get_player() const;
    const Bag& get_bag() const;
};