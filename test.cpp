#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "bullets.h"
#include "enemies.h"
#include "game_utils.h"
#include "players.h"

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

TEST_CASE("Random with same min max") { CHECK(generate_random(5, 5) == 5); }

TEST_CASE("Random invalid range throws") {
  CHECK_THROWS_AS(generate_random(10, 5), std::invalid_argument);
}
