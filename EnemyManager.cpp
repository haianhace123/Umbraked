#include "EnemyManager.h"
#include "Utils.h"
#include <algorithm>

EnemyManager::EnemyManager(std::vector<Enemy>& e, std::vector<Bullet>& eb, std::vector<Tile>& t) :
    enemies(e), enemyBullets(eb), tiles(t) {
}

void EnemyManager::updateEnemies(const SDL_Rect& playerRect, std::vector<Bullet>& playerBullets, int& lives, int& score,
                                 bool& isInvincible, Uint32& invincibilityTimer, float& playerVelY, Mix_Chunk* hitSound, Mix_Chunk* boomSound) {
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;

        enemy.velocityY += GRAVITY;
        enemy.rect.y += enemy.velocityY;

        bool onGround = false;
        for (const auto& tile : tiles) {
            if (checkCollision(enemy.rect, tile.rect)) {
                if (enemy.velocityY > 0 && enemy.rect.y + enemy.rect.h - enemy.velocityY <= tile.rect.y) {
                    enemy.rect.y = tile.rect.y - enemy.rect.h;
                    enemy.velocityY = 0;
                    onGround = true;
                } else if (enemy.velocityY < 0 && enemy.rect.y - enemy.velocityY >= tile.rect.y + tile.rect.h) {
                    enemy.rect.y = tile.rect.y + tile.rect.h;
                    enemy.velocityY = 0;
                }
            }
        }

        if (onGround) {
            int moveX = enemy.facingLeft ? -enemy.speed : enemy.speed;
            SDL_Rect futureRect = enemy.rect;
            futureRect.x += moveX;

            bool willCollide = false;
            for (const auto& tile : tiles) {
                if (checkCollision(futureRect, tile.rect)) {
                    willCollide = true;
                    break;
                }
            }

            SDL_Rect frontCheck = {enemy.facingLeft ? enemy.rect.x - 1 : enemy.rect.x + enemy.rect.w,
                                   enemy.rect.y + enemy.rect.h, 1, 1};
            bool hasPlatformAhead = false;
            for (const auto& tile : tiles) {
                if (checkCollision(frontCheck, tile.rect)) {
                    hasPlatformAhead = true;
                    break;
                }
            }

            if (willCollide || !hasPlatformAhead) {
                enemy.facingLeft = !enemy.facingLeft;
            } else {
                enemy.rect.x += moveX;
            }

            float distanceToPlayer = std::abs(playerRect.x + playerRect.w / 2 - (enemy.rect.x + enemy.rect.w / 2));
            if (distanceToPlayer < enemy.detectionRange && enemy.shootCooldown <= 0) {
                enemyBullets.push_back({{enemy.rect.x + (enemy.facingLeft ? 0 : enemy.rect.w), enemy.rect.y + enemy.rect.h / 2 - 2, 10, 5},
                                        (enemy.type <= 1) ? 3.0f : 4.0f, true, enemy.facingLeft, 0, enemy.rect.x + (enemy.facingLeft ? 0 : enemy.rect.w)});
                enemy.shootCooldown = (enemy.type <= 1) ? 40 : 30;
            } else if (enemy.shootCooldown > 0) {
                enemy.shootCooldown--;
            }
        }

        for (auto& bullet : playerBullets) {
            if (bullet.active && checkCollision(bullet.rect, enemy.rect)) {
                bullet.active = false;
                enemy.active = false;
                Mix_PlayChannel(-1, boomSound, 0);
                score += (enemy.type <= 1) ? 100 : 150;
                break;
            }
        }

        if (checkCollision(playerRect, enemy.rect) && !isInvincible) {
            if (playerRect.y + playerRect.h < enemy.rect.y + enemy.rect.h / 2 && playerVelY > 0) {
                enemy.active = false;
                playerVelY = JUMP_FORCE / 2; // Now in scope
                Mix_PlayChannel(-1, boomSound, 0);
                score += 50;
            } else {
                lives--;
                Mix_PlayChannel(-1, hitSound, 0);
                invincibilityTimer = SDL_GetTicks() + 2000;
                isInvincible = true;
            }
        }
    }

    for (auto& bullet : enemyBullets) {
        if (bullet.active) {
            bullet.rect.x += bullet.facingLeft ? -bullet.speed : bullet.speed;
            bullet.distanceTraveled = std::abs(bullet.rect.x - bullet.startX);
            if (bullet.distanceTraveled > BULLET_MAX_DISTANCE) bullet.active = false;
            for (const auto& tile : tiles) {
                if (checkCollision(bullet.rect, tile.rect)) { bullet.active = false; break; }
            }
            if (checkCollision(playerRect, bullet.rect) && !isInvincible) {
                bullet.active = false;
                lives--;
                Mix_PlayChannel(-1, hitSound, 0);
                invincibilityTimer = SDL_GetTicks() + 2000;
                isInvincible = true;
            }
        }
    }
}
