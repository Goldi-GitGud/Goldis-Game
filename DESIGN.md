# Design lock (T1) — defaults chosen so T2–T11 fit

1. Mode: endless waves only. No victory screen; death summary is the run end.
2. Boss/win: out of scope (a wave-N boss may layer on later without changing saves).
3. Death: permadeath. `Game::run` discards the run; `player_death` penalties stay inert.
4. Gold sink: YES — minimal camp shop (buy potions/one weapon, sell items/weapons).
5. Healing: camp heal costs 10g (+20 HP); free rest (+5 HP) if gold < 10.
6. Weapons: bag-stored, equippable in camp via `Player::change_weapon` (old weapon returns to bag).
7. Difficulty: global 1/2/3, changeable anytime; saved per slot but applies globally on load.
8. Saves: camp/pause only. A mid-wave save reloads at the SAME wave number but the
   wave restarts fresh (enemies are NOT persisted) — see note printed at save time.
9. Max HP: uncapped until T4 adds a cap; camp shows HP + weapon bonus meanwhile.
10. XP curve: `level*100` threshold (matches `Player::gain_experience`); T4 may retune.
