#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "bullets.h"
#include "enemies.h"
#include "game.h"
#include "game_utils.h"
#include "players.h"

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
        std::cout << "WIIIIIIIIIN\n";
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