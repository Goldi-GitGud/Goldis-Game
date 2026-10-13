# Terminal RPG (Goldi's Game)

## Build

Requires a C++17 compiler. From the `app/` directory:

```sh
g++ -std=c++17 -o start.exe start.cpp Game.cpp Menu.cpp Manager.cpp Combat.cpp Player.cpp Enemy.cpp Item.cpp Weapon.cpp Bag.cpp Utils.cpp
```

## Run

You MUST run from the `app/` directory (all JSON paths — `Player_data.json`,
`Enemy_data.json`, `Item_data.json`, `Weapon_data.json`, `save_slot_N.json` —
are relative to the working directory):

```sh
./start.exe        # Windows: start.exe
```

See `DESIGN.md` for scope decisions and `GAME_STATUS.md` for the task plan.
