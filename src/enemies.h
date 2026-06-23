#pragma once
#include <stdexcept>
#include <vector>

/**
 * @class Enemy
 * @brief Базовый класс врага, летящего навстречу игроку
 *
 * Враг появляется вверху экрана и движется вниз.
 * Столкновение с кораблём или выход за нижний край = проигрыш.
 */
class Enemy {
   protected:
    int x;                  /**< Координата X врага */
    int y;                  /**< Координата Y врага (увеличивается = движение вниз) */
    bool isLive;            /**< true если враг жив */
    bool hasAnimationShown; /**< true если анимация взрыва показана */

   public:
    /**
     * @brief Конструктор врага
     * @param x_ Начальная координата X
     * @param y_ Начальная координата Y (обычно 1 — верх экрана)
     */
    Enemy(int x_, int y_);

    /** @return Координата X */
    int getX() const;

    /** @return Координата Y */
    int getY() const;

    /** @return true если враг жив */
    bool state() const;

    /** @brief Уничтожить врага */
    virtual void kill();

    /**
     * @brief Установить позицию
     * @param new_x Новая X
     * @param new_y Новая Y
     */
    void set_position(int new_x, int new_y);

    /**
     * @brief Переместить врага вниз
     * @param turn Номер хода
     * @param level Текущая сложность (влияет на скорость)
     */
    void move_ememy(int turn, int level);

    /** @brief Уничтожить врага при попадании */
    void kill_enemy();

    /** @return true если враг уничтожен */
    bool is_destroyed() const;

    /** @return true если анимация показана */
    bool is_animation_shown() const;

    /** @brief Отметить анимацию как показанную */
    void show_animation();
};

/**
 * @class Boss
 * @brief Босс — усиленный враг с несколькими очками здоровья
 *
 * Появляется после определённого количества убитых врагов.
 * Требует нескольких попаданий для уничтожения.
 */
class Boss : public Enemy {
   private:
    int health; /**< Текущее здоровье босса */

   public:
    /**
     * @brief Конструктор босса
     * @param x_ Координата X
     * @param y_ Координата Y
     * @param hp Начальное здоровье (по умолчанию 5)
     * @throws std::invalid_argument Если hp <= 0
     */
    Boss(int x_, int y_, int hp = 5);

    /** @brief Нанести урон боссу (уменьшает health на 1) */
    void take_damage();

    /** @brief Уничтожить босса */
    void kill() override;

    /** @return Текущее здоровье */
    int GetHP() const;
};