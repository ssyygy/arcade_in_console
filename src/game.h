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
    /**
     * @brief Конструктор
     * @param reason Причина проигрыша (выводится игроку)
     */
    explicit GameOver(const std::string& reason) : std::runtime_error(reason) {}
};

/**
 * @brief Проверить столкновение корабля с врагами
 * @param spaceship Корабль игрока
 * @param enemies Список врагов
 * @throws GameOver Если корабль столкнулся с живым врагом
 */
void check_collision(Spaceship& spaceship, std::vector<Enemy>& enemies);

/**
 * @brief Проверить столкновение корабля с боссом
 * @param spaceship Корабль игрока
 * @param boss Объект босса
 * @param bossActive true, если босс сейчас активен на поле
 * @throws GameOver Если корабль столкнулся с боссом
 */
void check_boss_collision(Spaceship& spaceship, Boss& boss, bool bossActive);

/**
 * @brief Создать нового врага в случайной позиции в верхней части поля
 * @param enemies Список врагов, в который добавляется новый
 * @param level Текущий уровень сложности (должен быть > 0)
 * @throws std::invalid_argument Если level <= 0
 */
void spawn_enemy(std::vector<Enemy>& enemies, int level);

/**
 * @brief Переместить всех живых врагов на один шаг вниз
 * @param enemies Список врагов
 * @param turn Номер текущего хода (движение происходит раз в 3 хода)
 * @param level Текущий уровень сложности
 * @throws GameOver Если враг вышел за нижний край поля
 */
void move_enemies(std::vector<Enemy>& enemies, int turn, int level);

/**
 * @brief Переместить босса на один шаг вниз
 * @param boss Объект босса
 * @param turn Номер текущего хода (движение происходит раз в 3 хода)
 * @param level Текущий уровень сложности
 * @throws GameOver Если босс вышел за нижний край поля
 */
void move_boss(Boss& boss, int turn, int level);

/**
 * @brief Отрисовать текущее состояние игрового поля в консоль
 *
 * HUD занимает две строки: уровень и счёт; прогресс до босса или HP босса
 * с текстовой полоской.
 * @param bullets Вектор пуль игрока
 * @param enemies Вектор врагов
 * @param spaceship Корабль игрока
 * @param score Текущий счёт (количество попаданий)
 * @param boss Объект босса
 * @param bossActive true, если босс активен и должен быть отображён
 * @param totalKills Количество уничтоженных врагов на текущем уровне
 * @param currentLevel Номер текущего уровня (1–MAX_LEVEL)
 */
void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship,
                int score, Boss& boss, bool bossActive, int totalKills, int currentLevel);

/**
 * @brief Показать экран перехода между уровнями
 * @param level Номер уровня, который начинается
 * @param score Текущий счёт игрока
 */
void show_level_screen(int level, int score);