#ifndef GAME_H
#define GAME_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <random>
#include "Config.h"
#include "Structs.h"

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void close();
    TTF_Font* scoreFont;

private:
    void handleEvents();
    void update();
    void render();
    void renderText(const char* text, int x, int y, SDL_Color color, TTF_Font* font, bool center = false);
    void generateWorld();
    void resetGame();
    void fireBullet();
    void spawnEnemy(int x, int y);
    bool canSpawnEnemy(int x, int y, int width, int height);
    void updateEnemies();
    void cleanUpObjects();
    void loadBestScore();
    void saveBestScore();
    void updateMusic();
    void playSFX(Mix_Chunk* sound);
    bool isSpacePressed;
    float baseSpawnThreshold = 0.9f;
    float spawnThresholdDecrease = 0.03f;
    float currentSpawnThreshold;
    float baseBulletSpeed = 10.0f;
    float baseEnemyBulletSpeed = 3.0f;
    float bulletSpeedIncrease = 0.5f;
    int lastDifficultyThreshold = 0;
    int getNextDifficultyThreshold(int currentScore);
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* titleFont;
    SDL_Texture* playerTexture;
    SDL_Texture* groundTexture;
    SDL_Texture* floatingTexture;
    SDL_Texture* enemy1Texture;
    SDL_Texture* enemy2Texture;
    SDL_Texture* enemy3Texture;
    SDL_Texture* enemy4Texture;
    SDL_Texture* enemy5Texture;
    SDL_Texture* bulletTexture;
    SDL_Texture* enemyBulletTexture;
    SDL_Texture* backgroundTexture;
    SDL_Texture* spikeTexture;
    std::vector<SDL_Texture*> heartTextures;
    Mix_Chunk* hitSound;
    Mix_Chunk* shootSound;
    Mix_Chunk* boomSound;
    Mix_Chunk* jumpSound;
    Mix_Music* inGameMusic;
    Mix_Music* menuMusic;

    float enemy4AspectRatio;
    float enemy5AspectRatio;

    GameState gameState;
    bool isJumping;
    bool isOnGround;
    bool playerFlipped;
    float playerVelX;
    float playerVelY;
    int score;
    int bestScore;
    float cameraX;
    float maxPlayerX;
    int shootCooldown;
    int groundHeight;
    int lastGeneratedX;
    int lives;
    Uint32 invincibilityTimer;
    bool isInvincible;
    bool musicOn;
    bool sfxOn;

    SDL_Rect playerRect;
    std::vector<Tile> tiles;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;
    std::vector<Enemy> enemies;
    std::vector<SDL_Rect> spikes;
    std::vector<Button> menuButtons;

    std::mt19937 gen;
    std::uniform_int_distribution<> yDist;
    std::uniform_int_distribution<> gapDist;
    std::uniform_real_distribution<> spawnDist;
};

#endif
