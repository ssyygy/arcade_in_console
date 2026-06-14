#pragma once
#include <vector>

#include "enemies.h"

/**
 * @class Bullet
 * @brief Пуля, выпускаемая кораблём игрока
 *
 * Пуля летит вверх (вперёд) по направлению полёта.
 * При попадании во врага или босса — уничтожает его.
 */
class Bullet {
 private:
  int x;         /**< Координата X пули */
  int y;         /**< Координата Y пули (уменьшается = движение вверх) */
  bool isActive; /**< true если пуля летит */

 public:
  /** @brief Конструктор — создаёт неактивную пулю */
  Bullet();

  /** @return Координата X */
  int getX() const;

  /** @return Координата Y */
  int getY() const;

  /** @return true если пуля активна */
  bool isActiveStatus() const;

  /**
   * @brief Создать пулю из позиции корабля
   * @param x_now X корабля
   * @param y_now Y корабля (пуля появится выше на 1)
   */
  void create_bullet(int x_now, int y_now);

  /** @brief Переместить пулю вверх, деактивировать при выходе за экран */
  void move();

  /**
   * @brief Проверить попадание пули во врага или босса
   * @param enemies Список врагов
   * @param boss Объект босса
   * @param bossActive Флаг активности босса
   * @param global_counter Счётчик убийств (увеличивается при попадании)
   */
  void kill_enemy(std::vector<Enemy>& enemies, Boss& boss, bool& bossActive,
                  int& global_counter);
};
