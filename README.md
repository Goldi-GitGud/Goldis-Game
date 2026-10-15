# Goldi's Game — Terminal RPG

A turn-based, endless wave-crawler RPG for the terminal, written in C++17.
Pick a class, survive waves of enemies, earn XP and gold, and spend it at the camp shop. Death ends the run.

## Features

- 9 playable classes (Warrior, Knight, Rogue, Paladin, Berserker, Monk, Assassin, Duelist, Hunter) with distinct stats and starting weapons
- 13 enemies (Slime → Dragon) with level gating, wave scaling, and specials (regen, lifesteal, crits, bleed)
- Turn-based combat: attack, use items, target selection, dodge/armor mitigation
- Camp hub between waves: heal, equip weapons, shop (buy/sell), view stats/bag
- Level-gated shop: potions + 10 weapons, unlocks at Lv 5 / Lv 10, sell-back at 1/3 price
- XP and leveling (`level * 100` threshold), gold economy, weapon bonuses
- 3 save slots (JSON), pause/resume, Easy / Normal / Hard difficulty (changeable anytime)
- Data-driven stats: all classes, enemies, weapons, and items live in `data/*.json`

## Requirements

- C++17 compiler (MSVC, GCC, or Clang)
- CMake 3.16+
- A terminal (Windows, Linux, macOS)

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

The game loads `data/*_data.json` relative to the working directory, and `data/` is copied next to the binary on every build. Run from the binary's directory:

```sh
cd build
./start        # Windows: start.exe
```

Saves (`save_slot_*.json`) are created in the working directory and are git-ignored.

## Share a playable copy

```sh
cmake --install build --prefix dist
```

Zip the `dist/` folder (binary + `data/`). No compiler needed to play.

## How to play

1. Main menu → Start Game → New Game → pick a class + difficulty.
2. Camp → `Start wave` → fight until all enemies die.
3. Kills earn XP (level up) and gold. Between waves: heal, equip, shop.
4. Pause anytime to save (3 slots), change difficulty, or quit.
5. Mid-wave saves reload at the same wave number but the wave restarts fresh.
6. Death is permanent — the run summary is the end of that run.

In combat you choose `Attack / Use item / View stats / Pause` each turn, then pick a target. Potions heal or buff attack/armor/dodge until the wave ends.

## Project structure

```
app/
├── src/            # all sources (.h + .cpp per class) + start.cpp entry point
│   ├── Game.*      # run owner: camp hub, shop, heal, wave loop
│   ├── Combat.*    # one wave: spawning, turn loop, rewards
│   ├── Player.*    # classes, stats, XP/level, damage, heal
│   ├── Enemy.*     # enemy types, damage, XP/gold values
│   ├── Item.*      # 12 potions and their effects
│   ├── Weapon.*    # 10 weapons (stat bonuses)
│   ├── Bag.*       # inventory (items + weapons)
│   ├── Manager.*   # save/load for 3 slots
│   ├── Menu.*      # main / game / pause / options menus
│   └── Utils.*     # RNG + robust numeric input
├── data/           # Player_data.json, Enemy_data.json, Weapon_data.json, Item_data.json
├── CMakeLists.txt  # one add_executable over src/*.cpp, copies data/ next to binary
```

## Third-party

- `src/json.hpp` — [nlohmann/json](https://github.com/nlohmann/json) single-header library (MIT) for loading `data/` files and saves.
