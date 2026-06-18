#include "enemies.h"

#include <iostream>

#include "game_utils.h"

Enemy::Enemy(int x_, int y_) : x(x_), y(y_), isLive(true), hasAnimationShown(false) {}

int Enemy::getX() const {
    return x;
}
int Enemy::getY() const {
    return y;
}
bool Enemy::state() const {
    return isLive;
}

void Enemy::kill() {
    isLive = false;
    hasAnimationShown = false;
}

void Enemy::set_position(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Enemy::move_ememy(int turn, int level) {
    if (turn % 3 == 0 && isLive) {
        // way==1 — вправо, way==5 — влево, остальное — прямо вниз
        int way = generate_random(1, 5);
        if (way == 1) {
            x++;
            y++;
        } else if (way == 5) {
            x--;
            y++;
        } else {
            y++;
        }
    }
}

void Enemy::kill_enemy() {
    isLive = false;
    hasAnimationShown = false;
}

bool Enemy::is_destroyed() const {
    return !isLive;
}
bool Enemy::is_animation_shown() const {
    return hasAnimationShown;
}
void Enemy::show_animation() {
    hasAnimationShown = true;
}

Boss::Boss(int x_, int y_, int hp) : Enemy(x_, y_), health(hp) {}

void Boss::take_damage() {
    health--;
    if (health <= 0) {
        kill();
    }
}

void Boss::kill() {
    isLive = false;
}

int Boss::GetHP() const {
    return health;
}