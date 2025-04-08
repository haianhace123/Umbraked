#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H
#include <vector>
#include <random>
#include "Structs.h"
#include "Config.h"
class WorldGenerator {
public:
    WorldGenerator(std::vector<Tile>& tiles, int& groundHeight, int& lastGeneratedX, std::mt19937& gen);
    void generateWorld(const SDL_Rect& playerRect, std::vector<Enemy>& enemies);
private:
    std::vector<Tile>& tiles;
    int& groundHeight;
    int& lastGeneratedX;
    std::mt19937& gen;
    std::uniform_int_distribution<> yDist;
    std::uniform_int_distribution<> gapDist;
    std::uniform_real_distribution<> spawnDist;

    void spawnEnemy(int x, int y, std::vector<Enemy>& enemies);
    bool canSpawnEnemy(int x, int y, int width, int height);
};
#endif
