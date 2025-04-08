#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include "Config.h"
class ResourceManager {
public:
    ResourceManager(SDL_Renderer* renderer);
    ~ResourceManager();
    bool loadResources();
    void freeResources();
    SDL_Texture* getPlayerTexture() const { return playerTexture; }
    SDL_Texture* getGroundTexture() const { return groundTexture; }
    SDL_Texture* getFloatingTexture() const { return floatingTexture; }
    SDL_Texture* getEnemyTexture(int type) const;
    SDL_Texture* getBulletTexture() const { return bulletTexture; }
    SDL_Texture* getEnemyBulletTexture() const { return enemyBulletTexture; }
    SDL_Texture* getBackgroundTexture() const { return backgroundTexture; }
    SDL_Texture* getSpikeTexture() const { return spikeTexture; }
    SDL_Texture* getHeartTexture(int index) const { return heartTextures[index]; }
    Mix_Chunk* getHitSound() const { return hitSound; }
    Mix_Chunk* getShootSound() const { return shootSound; }
    Mix_Chunk* getBoomSound() const { return boomSound; }
    Mix_Chunk* getJumpSound() const { return jumpSound; }
    Mix_Music* getInGameMusic() const { return inGameMusic; }
    Mix_Music* getMenuMusic() const { return menuMusic; }
    TTF_Font* getFont() const { return font; }
    TTF_Font* getTitleFont() const { return titleFont; }
    float getEnemy4AspectRatio() const { return enemy4AspectRatio; }
    float getEnemy5AspectRatio() const { return enemy5AspectRatio; }
private:
    SDL_Renderer* renderer;
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
    TTF_Font* font;
    TTF_Font* titleFont;
    float enemy4AspectRatio;
    float enemy5AspectRatio;
};
#endif
