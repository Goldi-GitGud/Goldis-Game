# Terminal RPG (Goldi's Game)

Layout: `src/` holds all sources (each class's `.h` + `.cpp` together,
plus `start.cpp` and `json.hpp`); `data/` holds the `*_data.json` stat files.

## Build (g++, current)

Requires a C++17 compiler. From the `app/` directory:

```sh
g++ -std=c++17 -o start.exe src/start.cpp src/Game.cpp src/Menu.cpp src/Manager.cpp src/Combat.cpp src/Player.cpp src/Enemy.cpp src/Item.cpp src/Weapon.cpp src/Bag.cpp src/Utils.cpp
```

## Build (CMake, recommended)

```sh
cmake -S . -B build
cmake --build build
```

This produces `build/start` (`start.exe` on Windows) and copies `data/`
next to it. Run from the binary's own directory — saves
(`save_slot_*.json`, git-ignored) land in the working directory:

```sh
cd build && ./start        # Windows: start.exe
```

## Share with friends (install)

```sh
cmake --install build --prefix dist
```

This puts `start` + `data/` into `dist/`. Zip that folder and send it —
your friends just unzip and run it, no compiler needed. Saves
(`save_slot_*.json`) are created next to the game when they play.

## Run

You MUST run from the `app/` directory (data paths `data/*_data.json` and
runtime saves `save_slot_N.json` are relative to the working directory):

```sh
./start.exe        # Windows: start.exe
```

## Next: CMake

This layout is CMake-ready: one `add_executable` over `src/*.cpp` with
`target_include_directories(... src)`, plus copying `data/` next to the
binary (or setting the run working directory to `app/`). Saves stay in the
working directory and are git-ignored (`save_slot_*.json`).

See `DESIGN.md` for scope decisions and `GAME_STATUS.md` for the task plan.
