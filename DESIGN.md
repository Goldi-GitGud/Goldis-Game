# Design lock (T1) — defaults chosen so T2–T11 fit

1. Mode: endless waves only. No victory screen; death summary is the run end.
2. Boss/win: out of scope (a wave-N boss may layer on later without changing saves).
3. Death: permadeath. `Game::run` discards the run; `player_death` penalties stay inert.
4. Gold sink: YES — level-gated camp shop. LOW (Lv<5): Small potions + Sword/Dagger/Mace/Spear/Axe; MID (Lv5-9) adds Medium potions + Saber/Katana/Twinblade; HIGH (Lv10+) adds Large potions + Greatsword/Greataxe. Locked stock is shown with its unlock level and cannot be bought early. Buy prices: Small 10-15g, Medium 30g, Large 60g, basic weapons 40g, mid weapons 90g, top weapons 150g. Sell ANY bag item/weapon for 1/3 of its buy price (min 1g).
5. Healing: camp heal menu — 1. paid heal (cost 10 + effective_max_HP/10 gold, heals 30% of effective max, min 20) / 2. free rest (heals 5 + level HP) / 0. Back. Current/missing HP and both options' numbers are shown; at full HP nothing is charged.
6. Weapons: bag-stored, equippable in camp via `Player::change_weapon` (old weapon returns to bag).
7. Difficulty: global 1/2/3, changeable anytime; saved per slot but applies globally on load.
8. Saves: camp/pause only. A mid-wave save reloads at the SAME wave number but the
   wave restarts fresh (enemies are NOT persisted) — see note printed at save time.
9. Max HP: uncapped until T4 adds a cap; camp shows HP + weapon bonus meanwhile.
10. XP curve: `level*100` threshold (matches `Player::gain_experience`); T4 may retune.
