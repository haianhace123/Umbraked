#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H
#include <vector>
#include "Structs.h"
#include "Config.h"
#include <SDL_mixer.h>
class EnemyManager {
public:
    EnemyManager(std::vector<Enemy>& enemies, std::vector<Bullet>& enemyBullets, std::vector<Tile>& tiles);
    void updateEnemies(const SDL_Rect& playerRect, std::vector<Bullet>& playerBullets, int& lives, int& score,
                   bool& isInvincible, Uint32& invincibilityTimer, float& playerVelY, Mix_Chunk* hitSound, Mix_Chunk* boomSound);
private:
    std::vector<Enemy>& enemies;
    std::vector<Bullet>& enemyBullets;
    std::vector<Tile>& tiles;
};
#endif
