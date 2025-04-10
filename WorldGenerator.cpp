#include "WorldGenerator.h"
#include "Utils.h"
#include <cstdlib>

WorldGenerator::WorldGenerator(std::vector<Tile>& t, int& gh, int& lgx, std::mt19937& g) :
    tiles(t), groundHeight(gh), lastGeneratedX(lgx), gen(g),
    yDist(200, 400), gapDist(TILE_SIZE * 4, TILE_SIZE * 6), spawnDist(0.0f, 1.0f) {
}

void WorldGenerator::generateWorld(const SDL_Rect& playerRect, std::vector<Enemy>& enemies) {
    for (int y = 0; y < 3; y++) {
        for (int x = lastGeneratedX; x < lastGeneratedX + SCREEN_WIDTH + TILE_SIZE * 10; x += TILE_SIZE) {
            tiles.push_back({{x, y * TILE_SIZE, TILE_SIZE, TILE_SIZE}, true});
        }
    }

    if (spawnDist(gen) < 0.25f) {
        groundHeight += (rand() % 3 - 1) * TILE_SIZE;
        groundHeight = std::max(TILE_SIZE * 3, std::min(SCREEN_HEIGHT - TILE_SIZE * 3, groundHeight));
    }

    int segmentLength = TILE_SIZE * (rand() % 6 + 5);
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < segmentLength / TILE_SIZE; x++) {
            tiles.push_back({{lastGeneratedX + x * TILE_SIZE, groundHeight + y * TILE_SIZE, TILE_SIZE, TILE_SIZE}, true});
        }
    }
    if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.7f) {
        int spawnX = lastGeneratedX + segmentLength / 2;
        if (std::abs(spawnX - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
            spawnEnemy(spawnX, groundHeight - 40, enemies);
        }
    }
    lastGeneratedX += segmentLength;

    float roll = spawnDist(gen);
    if (roll < 0.2f) {
        int gapWidth = TILE_SIZE * (rand() % 3 + 2);
        if (gapWidth > MAX_JUMP_DISTANCE) {
            int numPlatforms = (gapWidth + MAX_JUMP_DISTANCE - 1) / MAX_JUMP_DISTANCE;
            int platformSpacing = gapWidth / (numPlatforms + 1);
            int prevY = groundHeight;
            for (int i = 1; i <= numPlatforms; i++) {
                int platformX = lastGeneratedX + platformSpacing * i;
                int platformY = prevY - TILE_SIZE * (rand() % 3 + 1);
                platformY = std::max(SCREEN_HEIGHT / 4, platformY);
                int platformWidth = TILE_SIZE * (rand() % 2 + 1);
                for (int j = 0; j < platformWidth / TILE_SIZE; j++) {
                    tiles.push_back({{platformX + j * TILE_SIZE, platformY, TILE_SIZE, TILE_SIZE}, false});
                }
                if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.5f && numPlatforms > 1 &&
                    std::abs(platformX + platformWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                    spawnEnemy(platformX + platformWidth / 2, platformY - 40, enemies);
                }
                prevY = platformY;
            }
        }
        lastGeneratedX += gapWidth;
    } else if (roll < 0.45f) {
        int pipeHeight = TILE_SIZE * (rand() % 3 + 2);
        int pipeWidth = TILE_SIZE * 2;
        for (int y = 0; y < pipeHeight / TILE_SIZE; y++) {
            for (int x = 0; x < pipeWidth / TILE_SIZE; x++) {
                tiles.push_back({{lastGeneratedX + x * TILE_SIZE, groundHeight - pipeHeight + y * TILE_SIZE, TILE_SIZE, TILE_SIZE}, true});
            }
        }
        if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.5f &&
            std::abs(lastGeneratedX + pipeWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
            spawnEnemy(lastGeneratedX + pipeWidth / 2, groundHeight - pipeHeight - 40, enemies);
        }
        lastGeneratedX += pipeWidth + TILE_SIZE * 2;
    } else if (roll < 0.85f) {
        int numPlatforms = rand() % 3 + 2;
        int totalWidth = TILE_SIZE * (rand() % 4 + 3);
        int platformSpacing = totalWidth / numPlatforms;
        int maxHeightDiff = 4 * TILE_SIZE;
        int prevX = lastGeneratedX;
        int prevY = groundHeight;
        for (int i = 0; i < numPlatforms; i++) {
            int platformX = prevX + platformSpacing;
            int platformY = prevY - TILE_SIZE * (rand() % 3 + 1);
            if (prevY - platformY > maxHeightDiff) platformY = prevY - maxHeightDiff;
            platformY = std::max(SCREEN_HEIGHT / 4, platformY);
            int platformWidth = TILE_SIZE * (rand() % 2 + 1);
            for (int j = 0; j < platformWidth / TILE_SIZE; j++) {
                tiles.push_back({{platformX + j * TILE_SIZE, platformY, TILE_SIZE, TILE_SIZE}, false});
            }
            if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.5f && numPlatforms > 1 &&
                std::abs(platformX + platformWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                spawnEnemy(platformX + platformWidth / 2, platformY - 40, enemies);
            }
            prevX = platformX + platformWidth;
            prevY = platformY;
        }
        lastGeneratedX += totalWidth + TILE_SIZE * 2;
    } else {
        if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.7f) {
            int spawnX = lastGeneratedX + TILE_SIZE * 2;
            if (std::abs(spawnX - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                spawnEnemy(spawnX, groundHeight - 40, enemies);
            }
        }
        lastGeneratedX += TILE_SIZE * 3;
    }

    if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < 0.5f) {
        int soloBlockX = lastGeneratedX - segmentLength / 2;
        int soloBlockY = groundHeight - TILE_SIZE * (rand() % 4 + 1);
        if (soloBlockY < groundHeight - 4 * TILE_SIZE) {
            tiles.push_back({{soloBlockX, soloBlockY, TILE_SIZE, TILE_SIZE}, false});
        }
    }
}

void WorldGenerator::spawnEnemy(int x, int y, std::vector<Enemy>& enemies) {
    Enemy enemy;
    enemy.type = rand() % 5;
    int baseHeight = (enemy.type == 3) ? 40 : 30;
<<<<<<< HEAD
    int width = (enemy.type == 3) ? static_cast<int>(baseHeight * 1.0f) :
=======
    int width = (enemy.type == 3) ? static_cast<int>(baseHeight * 1.0f) : 
>>>>>>> 6f6f4376087dd69e5b0099178b386bf39c4d4c87
                (enemy.type == 4) ? static_cast<int>(baseHeight * 1.0f) : 30;

    if (!canSpawnEnemy(x, y, width, baseHeight)) return;

    int adjustedY = y;
    for (const auto& tile : tiles) {
        if (tile.rect.x <= x + width && tile.rect.x + tile.rect.w >= x &&
            tile.rect.y >= y && tile.rect.y <= y + baseHeight + TILE_SIZE) {
            adjustedY = tile.rect.y - baseHeight;
            break;
        }
    }

    enemy.rect = {x, adjustedY, width, baseHeight};
    enemy.speed = (enemy.type <= 1) ? 1.5f : 2.0f;
    enemy.active = true;
    enemy.facingLeft = rand() % 2;
    enemy.shootCooldown = 0;
    enemy.detectionRange = 200.0f;
    enemy.velocityY = 0;
    enemies.push_back(enemy);
}

bool WorldGenerator::canSpawnEnemy(int x, int y, int width, int height) {
    SDL_Rect enemyRect = {x, y, width, height};
    SDL_Rect groundCheck = {x, y + height, width, TILE_SIZE};

    bool hasGround = false;
    for (const auto& tile : tiles) {
        if (checkCollision(groundCheck, tile.rect)) {
            hasGround = true;
            break;
        }
    }
    if (!hasGround) return false;

    for (const auto& tile : tiles) {
        if (checkCollision(enemyRect, tile.rect)) return false;
    }

    SDL_Rect leftCheck = {x - TILE_SIZE, y + height, TILE_SIZE, TILE_SIZE};
    SDL_Rect rightCheck = {x + width, y + height, TILE_SIZE, TILE_SIZE};

    bool hasLeftPlatform = false;
    bool hasRightPlatform = false;

    for (const auto& tile : tiles) {
        if (checkCollision(leftCheck, tile.rect)) hasLeftPlatform = true;
        if (checkCollision(rightCheck, tile.rect)) hasRightPlatform = true;
    }

    return hasLeftPlatform && hasRightPlatform;
}
