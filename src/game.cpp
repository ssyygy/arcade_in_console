#include "game.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "game_utils.h"

const int width = 41;
const int height = 41;
const int MAX_LEVEL = 5;

const LevelConfig LEVELS[MAX_LEVEL] = {
    {5, 3, 8, 3}, {8, 5, 6, 5}, {10, 7, 5, 7}, {12, 9, 4, 9}, {15, 12, 3, 12},
};

void check_collision(Spaceship& spaceship, std::vector<Enemy>& enemies) {
    for (auto& enemy : enemies) {
        if (enemy.state() && enemy.getX() == spaceship.getX() && enemy.getY() == spaceship.getY()) {
            throw GameOver("Ha-ha, you lost! The enemy flew into you!");
        }
    }
}

void check_boss_collision(Spaceship& spaceship, Boss& boss, bool bossActive) {
    if (bossActive && boss.state() && boss.getX() == spaceship.getX() &&
        boss.getY() == spaceship.getY()) {
        throw GameOver("Ha-ha, you lost! The BOSS flew into you!!");
    }
}

void spawn_enemy(std::vector<Enemy>& enemies, int level) {
    if (level <= 0) {
        throw std::invalid_argument("Level must be positive");
    }

    int x = generate_random(2, width - 3);
    int y = 1;

    // проверка, не занята ли позиция, чтобы враги не спавнились друг на друге
    bool position_taken = false;
    for (const auto& enemy : enemies) {
        if (enemy.state() && enemy.getX() == x && enemy.getY() == y) {
            position_taken = true;
            break;
        }
    }

    if (!position_taken) {
        enemies.emplace_back(x, y);
    }
}

void move_enemies(std::vector<Enemy>& enemies, int turn, int level) {
    if (turn % 3 != 0) return;

    for (auto& enemy : enemies) {
        if (!enemy.state()) continue;

        int new_y = enemy.getY() + 1;
        int new_x = enemy.getX();

        // way==1 - вправо, way==5 - влево, остальное - прямо вниз
        int way = generate_random(1, 5);
        if (way == 1 && new_x < width - 2)
            new_x++;
        else if (way == 5 && new_x > 1)
            new_x--;

        enemy.set_position(new_x, new_y);

        if (enemy.getY() >= height - 1) {
            throw GameOver("Ha-ha, you lost! The enemy flew into you!");
        }
    }
}

void move_boss(Boss& boss, int turn, int level) {
    if (turn % 3 != 0) return;

    int new_y = boss.getY() + 1;
    int new_x = boss.getX();

    // way==1 - вправо, way==5 - влево, остальное - прямо вниз
    int way = generate_random(1, 5);
    if (way == 1 && new_x < width - 2)
        new_x++;
    else if (way == 5 && new_x > 1)
        new_x--;

    boss.set_position(new_x, new_y);

    if (boss.getY() >= height - 1) {
        throw GameOver("Ha-ha, you lost! The BOSS flew past!");
    }
}

void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship,
                int score, Boss& boss, bool bossActive, int totalKills, int currentLevel) {
    clear_screen();

    std::string frame;
    // резервирую память заранее, чтобы избежать многократных реаллокаций
    frame.reserve(width * height * 4);

    const LevelConfig& cfg = LEVELS[currentLevel - 1];

    frame += "Level: ";
    frame += std::to_string(currentLevel);
    frame += "/";
    frame += std::to_string(MAX_LEVEL);
    frame += "   Score: ";
    frame += std::to_string(score);
    frame += '\n';

    if (bossActive) {
        frame += "*** BOSS ***  HP: ";
        frame += std::to_string(boss.GetHP());
        frame += "/";
        frame += std::to_string(cfg.boss_hp);
        frame += "  [";
        int barLen = 20;
        // целочисленное деление даёт количество заполненных сегментов полоски
        int filled = (boss.GetHP() * barLen) / cfg.boss_hp;
        for (int i = 0; i < barLen; i++) frame += (i < filled ? "#" : "-");
        frame += "]";
    } else {
        frame += "Kills: ";
        frame += std::to_string(totalKills);
        frame += "/";
        frame += std::to_string(cfg.kills_to_boss);
        frame += "  [";
        int barLen = 20;
        int filled = (totalKills * barLen) / cfg.kills_to_boss;
        for (int i = 0; i < barLen; i++) frame += (i < filled ? "#" : "-");
        frame += "]  -> BOSS";
    }
    frame += '\n';

    const int counterRow = spaceship.getY() + 2;

    for (int i = 0; i < height; i++) {
        if (i == counterRow) {
            frame += CHAR_BORDER;
            std::string counter;
            if (bossActive) {
                counter = " BOSS HP:" + std::to_string(boss.GetHP()) + "/" +
                          std::to_string(cfg.boss_hp) + " ";
            } else {
                counter = " Kills:" + std::to_string(totalKills) + "/" +
                          std::to_string(cfg.kills_to_boss) + " -> BOSS ";
            }
            int inner = width - 2;
            for (int k = 0; k < inner; k++) {
                if (k < (int)counter.size()) {
                    frame += counter[k];
                } else {
                    frame += CHAR_EMPTY;
                }
            }
            frame += CHAR_BORDER;
            frame += '\n';
            continue;
        }

        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                frame += CHAR_BORDER;
            } else if (i == spaceship.getY() && j == spaceship.getX()) {
                frame += CHAR_PLAYER;
            } else {
                bool drawn = false;

                for (Bullet& bullet : bullets) {
                    if (bullet.isActiveStatus() && bullet.getX() == j && bullet.getY() == i) {
                        frame += CHAR_BULLET;
                        drawn = true;
                        break;
                    }
                }

                if (!drawn) {
                    for (Enemy& enemy : enemies) {
                        if (enemy.state() && enemy.getX() == j && enemy.getY() == i) {
                            frame += CHAR_ENEMY;
                            drawn = true;
                            break;
                        } else if (enemy.is_destroyed() && !enemy.is_animation_shown() &&
                                   enemy.getX() <= j && enemy.getX() + 2 >= j &&
                                   enemy.getY() <= i && enemy.getY() + 2 >= i) {
                            // анимация взрыва занимает область 3x3 вокруг позиции врага
                            frame += CHAR_EXPLODE;
                            enemy.show_animation();
                            drawn = true;
                            break;
                        }
                    }
                }

                if (!drawn && bossActive && boss.state() && boss.getX() == j && boss.getY() == i) {
                    frame += CHAR_BOSS;
                    drawn = true;
                }

                if (!drawn) {
                    if (generate_random(1, 50) == 1) {
                        frame += CHAR_STAR;
                    } else {
                        frame += CHAR_EMPTY;
                    }
                }
            }
        }
        frame += '\n';
    }

    std::cout << frame << std::flush;
}

void show_level_screen(int level, int score) {
    clear_screen();
    std::cout << "=== LEVEL " << level << " ===\n";
    std::cout << "Score so far: " << score << "\n";
    std::cout << "Get ready...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}