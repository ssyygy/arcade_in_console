#include "players.h"

Spaceship::Spaceship() : x(17), y(31) {}

void Spaceship::move(char command) {
    if ((command == 'a' || command == 'A') && x - 1 > 0) x--;
    if ((command == 'd' || command == 'D') && x + 1 < 34) x++;
}

int Spaceship::getX() const {
    return x;
}
int Spaceship::getY() const {
    return y;
}