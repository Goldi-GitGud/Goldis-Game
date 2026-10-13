# Terminal RPG — Game Status, Gaps, and Task Plan

> Read-only survey. No code was edited to produce this file.
> Source dir: `C:\THE FOLDER\app`, entry: `start.cpp:1`, binary: `start.exe`.

## 1. What the game is

A terminal-based, endless wave-crawler RPG in C++ (with `json.hpp` single-header for data).

Core loop today:
```
Main Menu (`Menu.cpp:6`) -> New Game (choose 1 of 9 classes + difficulty) / Load Game (1 of 3 slots)
 -> Camp between waves (`Game.cpp:35`): Start wave / Use item / View stats / View bag / Pause
 -> Wave combat (`Combat.cpp:187`): turn-based Player turn -> Enemies turn until cleared / died / paused
 -> Rewards (XP + gold only) -> wave++ -> back to Camp, forever
 -> Pause menu (`Menu.cpp:82`): Resume / Save / Change difficulty / Quit to main / Exit
 -> Death: GAME OVER summary (`Game.cpp:81`), run discarded.
```

- Player stats (HP, Attack, Armor, Dodge, Level, XP, Gold) + Weapon bonuses. Data-driven from `Player_data.json`, `Weapon_data.json`, `Item_data.json`, `Enemy_data.json`.
- Enemies: 13 types gated by `MinLevel`. Count scales with wave. Difficulty is a global 1/2/3 (Easy 80%, Normal 100%, Hard 130%).
- Saves: one JSON per slot (`save_slot_<n>.json`), 3 slots via `Manager.cpp:15,58`.

## 2. Architecture map

| File | Owns / does |
|---|---|
| `start.cpp:6` | `main()`, constructs `Menu`, try/catch around everything |
| `Menu.h:6`, `Menu.cpp:6` | Main/Game/Pause/Options menus, `choose_class`, `choose_slot`, `offer_save`, `play` loop |
| `Game.h:10`, `Game.cpp:7` | Playthrough owner (`optional<Player>`, `Bag`, `optional<Combat>`, `wave`, `difficulty`), `new_game`, `load_state`, `run`, `camp` |
| `Combat.h:7`, `Combat.cpp:33` | One wave: `spawn_wave`, `fight`, `player_turn`, `enemies_turn`, `use_item_menu`, `reward_for` |
| `Player.h:8`, `Player.cpp:55` | 9-class enum, JSON-loaded base stats, attack/take_damage/dodge/level/heal/item/weapon methods |
| `Enemy.h:6`, `Enemy.cpp:22` | 13-type enum, JSON-loaded stats, attack/dodge, `exp = MinLevel*10`, `gold = MinLevel*5` |
| `Item.h:6`, `Item.cpp:21` | 12 potions (Small/Med/Large x Health/Defense/Dodge/Attack), `apply_effect` |
| `Weapon.h:4`, `Weapon.cpp:20` | 10 weapons, bonus-only stats, `display_stats` |
| `Bag.h:6`, `Bag.cpp:8` | `vector<Item>` + `vector<Weapon>`, add/remove/display |
| `Manager.h:6`, `Manager.cpp:15` | `save_game_saves`, `load_game_saves`, `describe_slot`, `slot_path` |
| `Utils.h:3`, `Utils.cpp:7` | `random_int`, `roll_chance`, `read_choice` (robust numeric input) |
| `Player_data.json` | 9 classes, all `Level:1, Gold:0`; 6 of 9 use `Sword` |
| `Enemy_data.json` | 13 enemies, `Hp/Attack/Armor/Dodge/MinLevel` (Slime 1 → Dragon 12) |
| `Weapon_data.json` | 10 weapons, `Health/Attack/Armor/DodgeBonus` |
| `Item_data.json` | 12 potions, `Health/Attack/Armor/DodgeBonus` |
| `save_slot_1.json` | Example save: Hunter L1, Wave 5, empty bag |

## 3. What is already coded and working

- [x] App boot + top-level exception guard (`start.cpp:8`).
- [x] Main menu / game menu / options / exit (`Menu.cpp:6,31,117,136`).
- [x] Class select with live stat preview from JSON (`Menu.cpp:168`).
- [x] Difficulty select Easy/Normal/Hard, changeable mid-run (`Menu.cpp:146`, `Game.cpp:106`, `Combat.cpp:27`).
- [x] New game init with placeholder starter items (`Game.cpp:9`).
- [x] Camp menu between waves (`Game.cpp:35`).
- [x] Wave spawn (count `1 + wave/2` cap 5, level-gated pool, difficulty % on HP/Attack) (`Combat.cpp:41`).
- [x] Turn loop: Attack / Use item / View stats / Pause; target picker; enemies attack in order (`Combat.cpp:144,176,98`).
- [x] Damage formulas: `Player::attack_enemy` (`Player.cpp:167`), `Player::take_damage` with dodge+armor (`Player.cpp:173`), `Enemy::attack_player` (`Enemy.cpp:97`), `Enemy::take_damage` with dodge (`Enemy.cpp:87`).
- [x] XP/level-up (`Player.cpp:186,233`), gold gain/loss (`Player.cpp:214,218`).
- [x] Potions: heal instant + Atk/Armor/Dodge to bonus stats until `clear_temporary_effects` at wave end (`Item.cpp:78`, `Player.cpp:208`).
- [x] Weapon swap bonus bookkeeping (`Player.cpp:195`).
- [x] Bag add/remove by type-match + display (`Bag.cpp:8,36`).
- [x] In-combat + camp item-use menu (`Combat.cpp:120`).
- [x] Pause mid-wave and resume same wave via `wave_active` flag (`Combat.cpp:35,187`, `Game.cpp:72`).
- [x] Save/load 3 slots with damaged-save handling (`Manager.cpp:15,58,98`).
- [x] Input hardening (`Utils.cpp:19`), RNG helpers (`Utils.cpp:7,13`).

## 4. What's remaining / needs editing (gaps, placeholders, bugs)

All `file:line` refs are where to fix later — nothing changed now.

1. **Combat scaling is placeholder.** Count formula + `difficulty_percent` only touches enemy HP/Attack (`Combat.cpp:27,44,63`). No per-wave stat growth, no armor/dodge scaling.
2. **Rewards are placeholder.** `Enemy.cpp:66,69` — flat `MinLevel*10 XP`, `MinLevel*5 gold`. No difficulty/wave scaling, no item/weapon drops. Gold has no sink (no shop).
3. **Progression is placeholder.** `Player.cpp:186` comment admits edge cases; `gain_level` (`Player.cpp:233`) is fixed `+20 HP/+5 Atk/+2 Armor/+1 Dodge`, threshold `level*100` with no cap/curve. `heal` (`Player.cpp:246`) has no max-HP cap → overheal.
4. **Death penalty is dead code.** `Player::player_death` (`Player.cpp:228`) halves XP and takes 40% gold, but `Game::run` (`Game.cpp:81`) discards the whole `Player` on death, so penalties never matter.
5. **Confusing damage messages.** `Player::take_damage` prints taken-after-armor (`Player.cpp:181`) then `Enemy::attack_player` prints raw attack (`Enemy.cpp:99`). Player sees two different numbers for one hit.
6. **Stat display is incomplete.** `Player::display_stats` (`Player.cpp:153`) shows base stats only, no bonus/total line. `Combat::show_status` (`Combat.cpp:83`) shows HP only, no armor/dodge/XP/gold/wave.
7. **Weapon system is stored but unusable.** `Bag` holds weapons (`Bag.cpp:11`), `Manager` saves them (`Manager.cpp:37`), but there is no UI to view/equip/swap/drop weapons in camp or combat, and no way to earn them. Most classes start on `Sword` (`Player_data.json:1`).
8. **No loot economy.** `reward_for` (`Combat.cpp:113`) gives only XP/gold. No drops, no shop, no camp healing/buying. Starter items (`Game.cpp:14`) are the only guaranteed items.
9. **Item UX gaps.** `use_item_menu` (`Combat.cpp:120`) lists names only, no effect text. `Bag::displayContents` (`Bag.cpp:36`) is single-line dump. No counts/grouping.
10. **Camp is thin.** `Game.cpp:35` shows `HP` with no max/level/gold/XP, no heal, no shop, no weapon equip.
11. **Save is lossy.** `Manager.cpp:22` saves base stats + wave but not `difficulty`, not `bonus` stats (by design), not mid-wave `enemies`/`wave_active`. Loading mid-wave restarts the wave fresh. No delete-slot, no overwrite confirm.
12. **No win condition / meta.** Endless waves only (`Game.cpp:66`). No boss wave, no victory, no high-score.
13. **Enemies have no variety in behavior.** All just melee-attack in vector order (`Combat.cpp:176`). No specials, crits, heals, flee.
14. **Data tuning unfinished.** 6/9 classes share `Sword`; `Dragon MinLevel 12` unreachable in normal play pacing; weapon/item numbers untested.
15. **Build/tooling missing.** No `Makefile`/`CMake`, relative JSON paths break if run outside `app/` (`Player.cpp:13`, `Enemy.cpp:14`, `Manager.cpp:11`), `start.exe` committed to repo, no tests.
16. **Text/polish nits.** `Menu.cpp:150` grammar (“you are a bit more stronger”), no How-to-Play screen.

## 5. Task breakdown (do in this order; each is testable in terminal)

- [ ] **T1 — Decide game scope (design lock).** Choose: endless-only vs win at wave N + boss; permadeath vs keep-save; gold sink (shop Y/N). Output: 10-line `DESIGN.md` addition. Unblocks T2–T6.
- [ ] **T2 — Fix combat numbers.** Replace `difficulty_percent` + flat count in `Combat.cpp:27,41` with wave-scaled HP/Atk (+ optionally Armor), cap-tested to wave 20. Add wave scaling + include armor/dodge in difficulty. Acceptance: Easy W1 killable, Hard W5 hard, no one-shot at W1.
- [ ] **T3 — Fix rewards + loot.** Extend `reward_for` (`Combat.cpp:113`) + `Enemy.cpp:66`: scale XP/gold by wave+difficulty, add % drop chance for items/weapons into `Bag`. Acceptance: clearing 3 waves visibly grows bag/gold.
- [ ] **T4 — Fix player progression.** Redefine `gain_experience`/`gain_level` curve (`Player.cpp:186,233`), add max-HP + cap `heal` (`Player.cpp:246`), show bonus/total in `display_stats` (`Player.cpp:153`). Decide fate of `player_death` penalties (`Player.cpp:228` vs `Game.cpp:81`). Acceptance: level-ups feel stepped, no overheal.
- [ ] **T5 — Make weapons usable.** Add equip/swap/view weapons UI in camp + combat (`Game.cpp:35`, `Combat.cpp:144`), wire `change_weapon` (`Player.cpp:195`). Seed starting weapon variety in `Player_data.json`. Acceptance: player can pick up, view, and equip a non-starter weapon.
- [ ] **T6 — Build camp into a real hub.** Extend `Game.cpp:35`: show HP/max, level, XP, gold, next wave; add Heal (cost or free-small), Shop (buy/sell with gold from T3), Equip. Acceptance: gold from T3 is spendable.
- [ ] **T7 — Fix combat UX text.** Unify damage message (remove double-print `Player.cpp:181` vs `Enemy.cpp:99`), enrich `show_status` (`Combat.cpp:83`) with enemy HP + player totals, add effect text to `use_item_menu` (`Combat.cpp:120`), group `Bag::displayContents` (`Bag.cpp:36`). Acceptance: one hit = one clear number.
- [ ] **T8 — Complete save/load.** Save `difficulty` + max-HP, decide mid-wave save (either block saving mid-wave or persist `enemies`+`wave_active`), add overwrite confirm + delete slot in `Menu.cpp:190`, `Manager.cpp:15`. Acceptance: save mid-wave → reload resumes correctly or cleanly restarts with warning.
- [ ] **T9 — Enemy variety (small).** Give 2–3 enemies a simple special (e.g. Troll regen, Vampire lifesteal, Lich high dodge) or crit chance; keep AI in `Combat.cpp:176` simple. Tune `Enemy_data.json`. Acceptance: at least 2 enemies feel different in log.
- [ ] **T10 — Menu/polish + data pass.** How-to-Play entry, fix `Menu.cpp:150` text, class/weapon variety pass over `Player_data.json`/`Weapon_data.json`/`Item_data.json`, remove `start.exe` from git + add build instructions (`Makefile` or `CMakeLists.txt` + README how to run so JSON paths resolve).
- [ ] **T11 — Smoke tests.** Manual script: New Game → W1–W3 → use item → pause → save slot 1 → exit → load → die → game over. Optional: add automated asserts for damage/XP/save round-trip. Acceptance: script passes on Easy + Normal without crash on bad input.

---
*Generated read-only; no `.cpp`/`.h`/`.json` files modified.*
