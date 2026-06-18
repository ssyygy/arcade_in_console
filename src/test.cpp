#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <stdexcept>

#include "bullets.h"
#include "enemies.h"
#include "game.h"
#include "game_utils.h"
#include "players.h"

// GameOver, check_collision, spawn_enemy, move_enemies — берутся из game.cpp
// через game.h, дублирование не нужно

TEST_CASE("Enemy creation and basic properties") {
    Enemy e(10, 5);
    CHECK(e.getX() == 10);
    CHECK(e.getY() == 5);
    CHECK(e.state() == true);
    CHECK(e.is_destroyed() == false);
}

TEST_CASE("Enemy kill and state change") {
    Enemy e(10, 5);
    e.kill_enemy();
    CHECK(e.state() == false);
    CHECK(e.is_destroyed() == true);
}

TEST_CASE("Enemy position setting") {
    Enemy e(10, 5);
    e.set_position(20, 15);
    CHECK(e.getX() == 20);
    CHECK(e.getY() == 15);
}

TEST_CASE("Enemy move down") {
    Enemy e(10, 5);
    int startY = e.getY();
    e.move_ememy(0, 1);
    CHECK(e.getY() > startY);
}

TEST_CASE("Boss creation and health") {
    Boss b(20, 5);
    CHECK(b.getX() == 20);
    CHECK(b.getY() == 5);
    CHECK(b.state() == true);
    CHECK(b.GetHP() == 5);
}

TEST_CASE("Boss take damage until death") {
    Boss b(20, 5);
    for (int i = 0; i < 4; i++) {
        b.take_damage();
        CHECK(b.state() == true);
    }
    b.take_damage();
    CHECK(b.state() == false);
    CHECK(b.GetHP() == 0);
}

TEST_CASE("Bullet lifecycle") {
    Bullet bullet;
    CHECK(bullet.isActiveStatus() == false);
    bullet.create_bullet(20, 30);
    CHECK(bullet.isActiveStatus() == true);
    CHECK(bullet.getX() == 20);
    CHECK(bullet.getY() == 29);
}

TEST_CASE("Bullet move and deactivate at top") {
    Bullet bullet;
    bullet.create_bullet(20, 2);
    bullet.move();
    CHECK(bullet.isActiveStatus() == false);
}

TEST_CASE("Bullet kill_enemy hits enemy") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, 10);
    Boss boss(0, 0);
    bool bossActive = false;
    int counter = 0;
    Bullet bullet;
    bullet.create_bullet(10, 11);
    bullet.kill_enemy(enemies, boss, bossActive, counter);
    CHECK(enemies[0].state() == false);
    CHECK(bullet.isActiveStatus() == false);
    CHECK(counter == 1);
}

TEST_CASE("Bullet kill_enemy misses enemy") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(5, 5);
    Boss boss(0, 0);
    bool bossActive = false;
    int counter = 0;
    Bullet bullet;
    bullet.create_bullet(10, 11);
    bullet.kill_enemy(enemies, boss, bossActive, counter);
    CHECK(enemies[0].state() == true);
    CHECK(bullet.isActiveStatus() == true);
    CHECK(counter == 0);
}

TEST_CASE("Bullet kill_enemy hits boss") {
    std::vector<Enemy> enemies;
    Boss boss(10, 10);
    bool bossActive = true;
    int counter = 0;
    Bullet bullet;
    bullet.create_bullet(10, 11);
    bullet.kill_enemy(enemies, boss, bossActive, counter);
    CHECK(boss.GetHP() == 4);
    CHECK(bullet.isActiveStatus() == false);
    CHECK(counter == 1);
}

TEST_CASE("Bullet kill_enemy does not hit inactive boss") {
    std::vector<Enemy> enemies;
    Boss boss(10, 10);
    bool bossActive = false;
    int counter = 0;
    Bullet bullet;
    bullet.create_bullet(10, 11);
    bullet.kill_enemy(enemies, boss, bossActive, counter);
    CHECK(boss.GetHP() == 5);
    CHECK(counter == 0);
}

TEST_CASE("Spaceship creation at center") {
    Spaceship s;
    CHECK(s.getX() == 19);
    CHECK(s.getY() == 37);
}

TEST_CASE("Spaceship move left") {
    Spaceship s;
    int startX = s.getX();
    s.move('a');
    CHECK(s.getX() == startX - 1);
}

TEST_CASE("Spaceship move right") {
    Spaceship s;
    int startX = s.getX();
    s.move('d');
    CHECK(s.getX() == startX + 1);
}

TEST_CASE("Spaceship left boundary") {
    Spaceship s;
    for (int i = 0; i < 25; i++) s.move('a');
    CHECK(s.getX() == 1);
    s.move('a');
    CHECK(s.getX() == 1);
}

TEST_CASE("Spaceship right boundary") {
    Spaceship s;
    for (int i = 0; i < 25; i++) s.move('d');
    CHECK(s.getX() == 39);
    s.move('d');
    CHECK(s.getX() == 39);
}

TEST_CASE("Random number in range") {
    for (int i = 0; i < 100; i++) {
        int val = generate_random(10, 20);
        CHECK(val >= 10);
        CHECK(val <= 20);
    }
}

TEST_CASE("Random with same min max") {
    CHECK(generate_random(5, 5) == 5);
}

TEST_CASE("Random invalid range throws") {
    CHECK_THROWS_AS(generate_random(10, 5), std::invalid_argument);
}

TEST_CASE("check_collision no collision") {
    Spaceship s;
    std::vector<Enemy> enemies;
    enemies.emplace_back(5, 5);
    CHECK_NOTHROW(check_collision(s, enemies));
}

TEST_CASE("check_collision throws on hit") {
    Spaceship s;
    std::vector<Enemy> enemies;
    enemies.emplace_back(s.getX(), s.getY());
    CHECK_THROWS_AS(check_collision(s, enemies), GameOver);
}

TEST_CASE("check_collision dead enemy does not trigger") {
    Spaceship s;
    std::vector<Enemy> enemies;
    enemies.emplace_back(s.getX(), s.getY());
    enemies[0].kill_enemy();
    CHECK_NOTHROW(check_collision(s, enemies));
}

TEST_CASE("spawn_enemy adds enemy to list") {
    std::vector<Enemy> enemies;
    spawn_enemy(enemies, 1);
    CHECK(enemies.size() >= 1);
}

TEST_CASE("spawn_enemy invalid level throws") {
    std::vector<Enemy> enemies;
    CHECK_THROWS_AS(spawn_enemy(enemies, 0), std::invalid_argument);
    CHECK_THROWS_AS(spawn_enemy(enemies, -1), std::invalid_argument);
}

TEST_CASE("move_enemies moves on turn divisible by 3") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, 5);
    int startY = enemies[0].getY();
    move_enemies(enemies, 0, 1);
    CHECK(enemies[0].getY() > startY);
}

TEST_CASE("move_enemies does not move on other turns") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, 5);
    int startY = enemies[0].getY();
    move_enemies(enemies, 1, 1);
    CHECK(enemies[0].getY() == startY);
}

TEST_CASE("move_enemies throws when enemy reaches bottom") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, 39);
    CHECK_THROWS_AS(move_enemies(enemies, 0, 1), GameOver);
}

TEST_CASE("move_enemies skips dead enemies") {
    std::vector<Enemy> enemies;
    enemies.emplace_back(10, 39);
    enemies[0].kill_enemy();
    CHECK_NOTHROW(move_enemies(enemies, 0, 1));
}