#include "players.h"

Spaceship::Spaceship() : x(19), y(37) {}

void Spaceship::move(char command) {
    if ((command == 'a' || command == 'A') && x - 1 > 0) x--;
    if ((command == 'd' || command == 'D') && x + 1 < 40) x++;
}

int Spaceship::getX() const {
    return x;
}
int Spaceship::getY() const {
    return y;
}