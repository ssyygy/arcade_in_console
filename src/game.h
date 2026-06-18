#pragma once
#include <stdexcept>
#include <vector>

#include "bullets.h"
#include "enemies.h"
#include "players.h"

/** @brief Ширина игрового поля (включая границы) */
extern const int width;

/** @brief Высота игрового поля (включая границы) */
extern const int height;

/** @brief Количество уровней в игре */
extern const int MAX_LEVEL;

/**
 * @brief Параметры одного уровня сложности
 */
struct LevelConfig {
    int kills_to_boss; /**< Сколько врагов нужно убить до появления босса */
    int max_enemies;   /**< Максимальное число живых врагов одновременно на экране */
    int spawn_every;   /**< Враг появляется каждые N ходов */
    int boss_hp;       /**< Здоровье босса на этом уровне */
};

/** @brief Конфигурации всех уровней */
extern const LevelConfig LEVELS[];

/**
 * @brief Исключение, генерируемое при проигрыше
 *
 * Бросается вместо exit() при любом условии поражения:
 * столкновении с врагом или боссом, либо когда враг пролетает мимо.
 */
class GameOver : public std::runtime_error {
   public:
    explicit GameOver(const std::string& reason) : std::runtime_error(reason) {}
};

void check_collision(Spaceship& spaceship, std::vector<Enemy>& enemies);
void check_boss_collision(Spaceship& spaceship, Boss& boss, bool bossActive);
void spawn_enemy(std::vector<Enemy>& enemies, int level);
void move_enemies(std::vector<Enemy>& enemies, int turn, int level);
void move_boss(Boss& boss, int turn, int level);
void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship,
                int score, Boss& boss, bool bossActive, int totalKills, int currentLevel);
void show_level_screen(int level, int score);