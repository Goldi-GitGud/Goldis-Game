#include "Utils.h"
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

int random_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

bool roll_chance(int percent) {
    return random_int(1, 100) <= percent;
}

// Without this check, typing letters makes cin fail and every later cin >> does nothing,
// which would make a menu loop forever.
int read_choice(int min, int max) {
    while (true) {
        int choice;
        if (std::cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        if (std::cin.eof()) {
            throw std::runtime_error("Input closed");
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number from " << min << " to " << max << ": ";
    }
}