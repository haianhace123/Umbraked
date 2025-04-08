#include "ResourceManager.h"
#include <SDL_image.h>
#include <cstdio>

ResourceManager::ResourceManager(SDL_Renderer* rend) : renderer(rend),
    playerTexture(nullptr), groundTexture(nullptr), floatingTexture(nullptr),
    enemy1Texture(nullptr), enemy2Texture(nullptr), enemy3Texture(nullptr),
    enemy4Texture(nullptr), enemy5Texture(nullptr), bulletTexture(nullptr),
    enemyBulletTexture(nullptr), backgroundTexture(nullptr), spikeTexture(nullptr),
    hitSound(nullptr), shootSound(nullptr), boomSound(nullptr), jumpSound(nullptr),
    inGameMusic(nullptr), menuMusic(nullptr), font(nullptr), titleFont(nullptr),
    enemy4AspectRatio(1.0f), enemy5AspectRatio(1.0f) {
}

ResourceManager::~ResourceManager() {
    freeResources();
}

bool ResourceManager::loadResources() {
    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    titleFont = TTF_OpenFont(TITLE_FONT_PATH, 72);
    if (!font || !titleFont) return false;

    SDL_Surface* surface;
    surface = IMG_Load(PLAYER_IMAGE_PATH); playerTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(GROUND_TILE_PATH); groundTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(FLOATING_TILE_PATH); floatingTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(ENEMY1_IMAGE_PATH); enemy1Texture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(ENEMY2_IMAGE_PATH); enemy2Texture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(ENEMY3_IMAGE_PATH); enemy3Texture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);

    surface = IMG_Load(ENEMY4_IMAGE_PATH);
    enemy4Texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (surface) {
        enemy4AspectRatio = static_cast<float>(surface->w) / surface->h;
        SDL_FreeSurface(surface);
    } else {
        printf("Failed to load enemy4.png! SDL_Error: %s\n", SDL_GetError());
        enemy4AspectRatio = 1.0f;
    }

    surface = IMG_Load(ENEMY5_IMAGE_PATH);
    enemy5Texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (surface) {
        enemy5AspectRatio = static_cast<float>(surface->w) / surface->h;
        SDL_FreeSurface(surface);
    } else {
        printf("Failed to load enemy5.png! SDL_Error: %s\n", SDL_GetError());
        enemy5AspectRatio = 1.0f;
    }

    surface = IMG_Load(BULLET_IMAGE_PATH); bulletTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(ENEMY_BULLET_IMAGE_PATH); enemyBulletTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(BACKGROUND_IMAGE_PATH); backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);
    surface = IMG_Load(SPIKE_IMAGE_PATH); spikeTexture = SDL_CreateTextureFromSurface(renderer, surface); SDL_FreeSurface(surface);

    surface = IMG_Load(LIVE_IMAGE_PATH);
    heartTextures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    heartTextures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    heartTextures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    SDL_FreeSurface(surface);
    surface = IMG_Load(DIE_IMAGE_PATH);
    heartTextures.push_back(SDL_CreateTextureFromSurface(renderer, surface));
    SDL_FreeSurface(surface);

    hitSound = Mix_LoadWAV(HIT_SOUND_PATH);
    shootSound = Mix_LoadWAV(SHOOT_SOUND_PATH);
    boomSound = Mix_LoadWAV(BOOM_SOUND_PATH);
    jumpSound = Mix_LoadWAV(JUMP_SOUND_PATH);
    inGameMusic = Mix_LoadMUS(INGAME_SOUND_PATH);
    menuMusic = Mix_LoadMUS(MENU_SOUND_PATH);

    Mix_VolumeMusic(64);

    return playerTexture && groundTexture && floatingTexture && enemy1Texture && enemy2Texture &&
           enemy3Texture && enemy4Texture && enemy5Texture && bulletTexture && enemyBulletTexture &&
           backgroundTexture && spikeTexture && heartTextures.size() == 4 && hitSound && shootSound &&
           boomSound && jumpSound && inGameMusic && menuMusic;
}

void ResourceManager::freeResources() {
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(floatingTexture);
    SDL_DestroyTexture(enemy1Texture);
    SDL_DestroyTexture(enemy2Texture);
    SDL_DestroyTexture(enemy3Texture);
    SDL_DestroyTexture(enemy4Texture);
    SDL_DestroyTexture(enemy5Texture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(enemyBulletTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(spikeTexture);
    for (auto texture : heartTextures) SDL_DestroyTexture(texture);
    heartTextures.clear();
    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(boomSound);
    Mix_FreeChunk(jumpSound);
    Mix_FreeMusic(inGameMusic);
    Mix_FreeMusic(menuMusic);
    TTF_CloseFont(font);
    TTF_CloseFont(titleFont);
}

SDL_Texture* ResourceManager::getEnemyTexture(int type) const {
    switch (type) {
        case 0: return enemy1Texture;
        case 1: return enemy2Texture;
        case 2: return enemy3Texture;
        case 3: return enemy4Texture;
        case 4: return enemy5Texture;
        default: return nullptr;
    }
}
