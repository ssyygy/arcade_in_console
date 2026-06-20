#include "game.h"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "game_utils.h"

const int width = 35;
const int height = 35;
const int MAX_LEVEL = 5;

const LevelConfig LEVELS[MAX_LEVEL] = {
    {5, 3, 8, 3}, {8, 5, 6, 5}, {10, 7, 5, 7}, {12, 9, 4, 9}, {15, 12, 3, 12},
};

bool step_down(int& x, int& y) {
    y++;

    int way = generate_random(1, 5);
    if (way == 1 && x < width - 2)
        x++;
    else if (way == 5 && x > 1)
        x--;

    return y >= height - 1;
}

bool check_collision(Spaceship& spaceship, std::vector<Enemy>& enemies) {
    for (auto& enemy : enemies) {
        if (enemy.state() && enemy.getX() == spaceship.getX() && enemy.getY() == spaceship.getY()) {
            return true;  // враг столкнулся с кораблём — игра окончена
        }
    }
    return false;
}

bool check_boss_collision(Spaceship& spaceship, Boss& boss, bool bossActive) {
    if (bossActive && boss.state() && boss.getX() == spaceship.getX() &&
        boss.getY() == spaceship.getY()) {
        return true;  // босс столкнулся с кораблём — игра окончена
    }
    return false;
}

void spawn_enemy(std::vector<Enemy>& enemies, int level) {
    if (level <= 0) {
        throw std::invalid_argument("Level must be positive");
    }

    int x = generate_random(2, width - 3);
    int y = 1;

    // чтобы враги не спавнились друг на друге
    for (const auto& enemy : enemies) {
        if (enemy.state() && enemy.getX() == x && enemy.getY() == y) {
            return;  // место занято — просто не спавним в этот раз
        }
    }

    enemies.emplace_back(x, y);
}

bool move_enemies(std::vector<Enemy>& enemies, int turn, int level) {
    if (turn % 3 != 0) return false;

    for (auto& enemy : enemies) {
        if (!enemy.state()) continue;

        int new_x = enemy.getX();
        int new_y = enemy.getY();
        bool reachedBottom = step_down(new_x, new_y);

        enemy.set_position(new_x, new_y);

        if (reachedBottom) {
            return true;  // враг долетел до низа — игра окончена
        }
    }
    return false;
}

bool move_boss(Boss& boss, int turn, int level) {
    if (turn % 3 != 0) return false;

    int new_x = boss.getX();
    int new_y = boss.getY();
    bool reachedBottom = step_down(new_x, new_y);

    boss.set_position(new_x, new_y);

    return reachedBottom;
}

void draw_field(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Spaceship& spaceship,
                int score, Boss& boss, bool bossActive, int totalKills, int currentLevel) {
    clear_screen();

    std::string frame;
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

void show_game_over_screen(const std::string& reason) {
    clear_screen();
    std::cout << reason << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}