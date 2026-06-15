#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "bullets.h"
#include "enemies.h"
#include "game.h"
#include "game_utils.h"
#include "players.h"

const int width = 41;
const int height = 41;
const int MAX_LEVEL = 5;

static const LevelConfig LEVELS[MAX_LEVEL] = {
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
            throw GameOver("Porazhenie! Vrag proletel mimo!");
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

/**
 * @brief заставка между уровнями
 * @param level номер следующего уровня
 * @param score текущий счёт
 */
void show_level_screen(int level, int score) {
    clear_screen();
    std::cout << "=== LEVEL " << level << " ===\n";
    std::cout << "Score so far: " << score << "\n";
    std::cout << "Get ready...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main() {
    try {
        setup_console();

        int score = 0;
        int currentLevel = 1;

        show_level_screen(currentLevel, score);

        while (currentLevel <= MAX_LEVEL) {
            const LevelConfig& cfg = LEVELS[currentLevel - 1];

            int totalKills = 0;
            int turn = 0;
            bool bossActive = false;
            bool bossDefeated = false;
            Boss boss(20, 2);
            std::vector<Enemy> enemies;
            std::vector<Bullet> bullets;
            Spaceship spaceship;

            for (int i = 0; i < std::min(3, cfg.max_enemies); i++) {
                spawn_enemy(enemies, currentLevel);
            }

            draw_field(bullets, enemies, spaceship, score, boss, bossActive, totalKills,
                       currentLevel);

            auto last_update = std::chrono::steady_clock::now();

            while (true) {
                char command = getch_nonblock();

                if (command == 'a' || command == 'A' || command == 'd' || command == 'D') {
                    spaceship.move(command);
                }
                if (command == ' ') {
                    Bullet new_bullet;
                    new_bullet.create_bullet(spaceship.getX(), spaceship.getY());
                    bullets.push_back(new_bullet);
                }

                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update)
                        .count() >= 150) {
                    for (auto& bullet : bullets) {
                        // считаю живых до и после выстрела, чтобы засчитать kill
                        int liveBefore = 0;
                        for (const auto& e : enemies) {
                            if (e.state()) liveBefore++;
                        }

                        bullet.kill_enemy(enemies, boss, bossActive, score);

                        int liveAfter = 0;
                        for (const auto& e : enemies) {
                            if (e.state()) liveAfter++;
                        }

                        if (liveAfter < liveBefore) totalKills++;

                        bullet.move();
                    }
                    // erase-remove idiom: удаляю неактивные пули из вектора
                    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                                 [](Bullet& b) { return !b.isActiveStatus(); }),
                                  bullets.end());

                    if (bossActive && !boss.state()) {
                        bossDefeated = true;
                        bossActive = false;
                        score += currentLevel * 10;
                        currentLevel++;
                        break;
                    }

                    int liveEnemies = 0;
                    for (const auto& e : enemies) {
                        if (e.state()) liveEnemies++;
                    }

                    if (!bossActive && !bossDefeated && liveEnemies < cfg.max_enemies &&
                        turn % cfg.spawn_every == 0) {
                        spawn_enemy(enemies, currentLevel);
                    }

                    move_enemies(enemies, turn, currentLevel);
                    check_collision(spaceship, enemies);

                    if (!bossDefeated && !bossActive && totalKills >= cfg.kills_to_boss) {
                        bossActive = true;
                        enemies.clear();
                        bullets.clear();
                        boss = Boss(generate_random(10, width - 10), 2, cfg.boss_hp);
                    }

                    if (bossActive && boss.state()) {
                        move_boss(boss, turn, currentLevel);
                        check_boss_collision(spaceship, boss, bossActive);
                    }

                    draw_field(bullets, enemies, spaceship, score, boss, bossActive, totalKills,
                               currentLevel);
                    turn++;
                    last_update = now;
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            if (currentLevel <= MAX_LEVEL) {
                show_level_screen(currentLevel, score);
            }
        }

        clear_screen();
        std::cout << "POBEDA! Vse urovni projdeny!\n";
        std::cout << "Final score: " << score << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 0;

    } catch (const GameOver& e) {
        clear_screen();
        std::cout << e.what() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}