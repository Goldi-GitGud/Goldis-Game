#pragma once

int random_int(int min, int max);
bool roll_chance(int percent);   // true with the given % chance
int read_choice(int min, int max); // asks the player for a whole number between min and max (inclusive), repeating until the input is valid