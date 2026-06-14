#pragma once

/**
 * @class Spaceship
 * @brief Космический корабль игрока
 *
 * Корабль летит вперёд (вверх по экрану).
 * Управление: A/D — влево/вправо, Space — стрелять.
 * Столкновение с врагом = проигрыш.
 */
class Spaceship {
 private:
  int x; /**< Координата X (может меняться) */
  int y; /**< Координата Y (постоянна, корабль летит вперёд) */

 public:
  /** @brief Конструктор — создаёт корабль в центре нижней части */
  Spaceship();

  /**
   * @brief Переместить корабль влево/вправо
   * @param command 'a'/'A' — влево, 'd'/'D' — вправо
   */
  void move(char command);

  /** @return Координата X */
  int getX() const;

  /** @return Координата Y */
  int getY() const;
};
