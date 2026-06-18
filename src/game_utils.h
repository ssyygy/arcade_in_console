#pragma once

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#endif

/**
 * @brief Получить нажатие клавиши без блокировки
 * @return Символ нажатой клавиши или 0, если ничего не нажато
 */
char getch_nonblock();

/**
 * @brief Очистить экран или переместить курсор в начало
 */
void clear_screen();

/**
 * @brief Настроить консоль для игры
 */
void setup_console();

/**
 * @brief Сгенерировать случайное целое число
 * @param min Минимальное значение (включительно)
 * @param max Максимальное значение (включительно)
 * @return Случайное число в диапазоне [min, max]
 */
int generate_random(int min, int max);

#ifdef _WIN32
// ASCII для Windows (консоль Windows плохо поддерживает эмодзи)
#define CHAR_BORDER '#'
#define CHAR_PLAYER 'A'  // Aircraft
#define CHAR_BULLET '|'
#define CHAR_ENEMY 'V'  // Enemy flying down
#define CHAR_EXPLODE 'X'
#define CHAR_BOSS 'B'
#define CHAR_EMPTY ' '
#define CHAR_STAR '.'
#else
// Эмодзи для macOS/Linux
#define CHAR_BORDER "\U0001F30C"   // 🌌
#define CHAR_PLAYER "\U0001F6F8"   // 🛸
#define CHAR_BULLET "\U0001F525"   // 🔥
#define CHAR_ENEMY "\U0001F47D"    // 👽
#define CHAR_EXPLODE "\U0001F4A5"  // 💥
#define CHAR_BOSS "\U0001F480"     // 💀
#define CHAR_EMPTY "  "
#define CHAR_STAR " ."
#endif