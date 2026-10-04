#include "Utils.h"
#include <random>

int random_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

bool roll_chance(int percent) {
    return random_int(1, 100) <= percent;
}