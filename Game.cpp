#include "Game.h"
#include "Utils.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstdio>

Game::Game() :
    window(nullptr), renderer(nullptr), font(nullptr), titleFont(nullptr), scoreFont(nullptr),
    playerTexture(nullptr), groundTexture(nullptr), floatingTexture(nullptr),
    enemy1Texture(nullptr), enemy2Texture(nullptr), enemy3Texture(nullptr),
    enemy4Texture(nullptr), enemy5Texture(nullptr), bulletTexture(nullptr),
    enemyBulletTexture(nullptr), backgroundTexture(nullptr), spikeTexture(nullptr),
    hitSound(nullptr), shootSound(nullptr), boomSound(nullptr), jumpSound(nullptr),
    inGameMusic(nullptr), menuMusic(nullptr),
    enemy4AspectRatio(1.0f), enemy5AspectRatio(1.0f),
    gameState(MAIN_MENU), isJumping(false), isOnGround(true),
    playerFlipped(false), playerVelX(0), playerVelY(0), score(0), bestScore(0),
    cameraX(0), maxPlayerX(0), shootCooldown(0), groundHeight(GROUND_HEIGHT),
    lastGeneratedX(0), lives(3), invincibilityTimer(0), isInvincible(false),
    musicOn(true), sfxOn(true), isSpacePressed(false),
    playerRect{SCREEN_WIDTH / 4, GROUND_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT},
    gen(std::random_device()()),
    yDist(200, 400), gapDist(TILE_SIZE * 4, TILE_SIZE * 6), spawnDist(0.0f, 1.0f) {
}

Game::~Game() {
    close();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) || TTF_Init() == -1) {
        printf("SDL initialization failed! Error: %s\n", SDL_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    window = SDL_CreateWindow("Umbraked", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!window || !renderer) return false;

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    titleFont = TTF_OpenFont(TITLE_FONT_PATH, 72);
    scoreFont = TTF_OpenFont("txt/BungeeTint-Regular.ttf", 28);
    if (!font || !titleFont || !scoreFont) {
        printf("Failed to load fonts! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

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

    if (!playerTexture || !groundTexture || !floatingTexture || !enemy1Texture || !enemy2Texture ||
        !enemy3Texture || !enemy4Texture || !enemy5Texture || !bulletTexture || !enemyBulletTexture ||
        !backgroundTexture || !spikeTexture || heartTextures.size() < 4 || !hitSound || !shootSound ||
        !boomSound || !jumpSound || !inGameMusic || !menuMusic) {
        printf("Failed to load resources!\n");
        return false;
    }

    loadBestScore();
    resetGame();
    if (musicOn) Mix_PlayMusic(menuMusic, -1);

    return true;
}

void Game::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    while (true) {
        frameStart = SDL_GetTicks();
        handleEvents();
        if (gameState == PLAYING) update();
        updateMusic();
        render();
        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) { close(); exit(0); }
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x, y = e.button.y;
            if (gameState == MAIN_MENU) {
                if (checkCollision({x, y, 1, 1}, menuButtons[0].rect)) {
                    resetGame();
                    gameState = PLAYING;
                    if (musicOn) { Mix_HaltMusic(); Mix_PlayMusic(inGameMusic, -1); }
                } else if (checkCollision({x, y, 1, 1}, menuButtons[1].rect)) gameState = INSTRUCTIONS;
                else if (checkCollision({x, y, 1, 1}, menuButtons[2].rect)) gameState = RECORDS;
                else if (checkCollision({x, y, 1, 1}, menuButtons[3].rect)) gameState = OPTIONS;
            } else if (gameState == INSTRUCTIONS || gameState == RECORDS) {
                if (checkCollision({x, y, 1, 1}, menuButtons[0].rect)) gameState = MAIN_MENU;
            } else if (gameState == OPTIONS) {
                if (checkCollision({x, y, 1, 1}, menuButtons[0].rect)) {
                    musicOn = !musicOn;
                    if (!musicOn) Mix_HaltMusic(); else Mix_PlayMusic(menuMusic, -1);
                } else if (checkCollision({x, y, 1, 1}, menuButtons[1].rect)) sfxOn = !sfxOn;
                else if (checkCollision({x, y, 1, 1}, menuButtons[2].rect)) gameState = MAIN_MENU;
            } else if (gameState == GAME_OVER) {
                if (checkCollision({x, y, 1, 1}, menuButtons[0].rect)) {
                    resetGame();
                    gameState = PLAYING;
                    if (musicOn) Mix_PlayMusic(inGameMusic, -1);
                } else if (checkCollision({x, y, 1, 1}, menuButtons[1].rect)) gameState = MAIN_MENU;
                else if (checkCollision({x, y, 1, 1}, menuButtons[2].rect)) { close(); exit(0); }
            }
        }
        if (gameState == PLAYING && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE && shootCooldown <= 0 && !isSpacePressed) {
            fireBullet();
            shootCooldown = 13;
            isSpacePressed = true;
        }

        if (gameState == PLAYING && e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_SPACE) {
            isSpacePressed = false;
        }
    }

    if (gameState == PLAYING) {
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        playerVelX = 0.0f;
        if (keystate[SDL_SCANCODE_A]) { playerVelX -= PLAYER_SPEED; playerFlipped = true; }
        if (keystate[SDL_SCANCODE_D]) { playerVelX += PLAYER_SPEED; playerFlipped = false; }
        if (keystate[SDL_SCANCODE_W] && isOnGround && !isJumping) {
            isJumping = true; isOnGround = false; playerVelY = JUMP_FORCE; playSFX(jumpSound);
        }
    }
}

void Game::update() {
    if (shootCooldown > 0) shootCooldown--;

    playerVelY += GRAVITY;
    SDL_Rect futureRect = playerRect;
    futureRect.x += playerVelX;
    futureRect.y += playerVelY;

    cameraX += CAMERA_SPEED;

    static float spawnThreshold = 0.9f;
    static float enemyBulletSpeed = 3.0f;
    static int lastThreshold = 0;
    int nextThreshold = getNextDifficultyThreshold(lastThreshold);
    if (score >= nextThreshold && score >= 100) {
        spawnThreshold = 0.9f - 0.03f * (score / 100);
        spawnThreshold = std::max(0.3f, spawnThreshold);
        enemyBulletSpeed += 0.1f;
        lastThreshold = nextThreshold;
    }

    isOnGround = false;
    bool horizontalCollision = false;

    for (const auto& tile : tiles) {
        if (checkCollision(futureRect, tile.rect)) {
            if (playerVelY > 0 && playerRect.y + playerRect.h <= tile.rect.y) {
                playerRect.y = tile.rect.y - playerRect.h;
                playerVelY = 0;
                isOnGround = true;
                isJumping = false;
            } else if (playerVelY < 0 && playerRect.y >= tile.rect.y + tile.rect.h) {
                playerRect.y = tile.rect.y + tile.rect.h;
                playerVelY = 0;
            } else if (playerVelX != 0 && playerRect.y + playerRect.h > tile.rect.y && playerRect.y < tile.rect.y + tile.rect.h) {
                if (playerVelX > 0 && playerRect.x + playerRect.w <= tile.rect.x) {
                    playerRect.x = tile.rect.x - playerRect.w;
                    horizontalCollision = true;
                    if (isJumping && playerVelY < 0) playerRect.y += playerVelY;
                } else if (playerVelX < 0 && playerRect.x >= tile.rect.x + tile.rect.w) {
                    playerRect.x = tile.rect.x + tile.rect.w;
                    horizontalCollision = true;
                    if (isJumping && playerVelY < 0) playerRect.y += playerVelY;
                }
            }
        }
    }

    if (!horizontalCollision) playerRect.x += playerVelX;
    if (!isOnGround && (!horizontalCollision || (horizontalCollision && playerVelY >= 0))) playerRect.y += playerVelY;

    if (playerRect.x < cameraX + TILE_SIZE) playerRect.x = cameraX + TILE_SIZE;
    if (playerRect.x > maxPlayerX) {
        maxPlayerX = playerRect.x;
        score = static_cast<int>(maxPlayerX / 10);
    }

    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.rect.x += bullet.facingLeft ? -bullet.speed : bullet.speed;
            bullet.distanceTraveled = std::abs(bullet.rect.x - bullet.startX);
            if (bullet.distanceTraveled > BULLET_MAX_DISTANCE) bullet.active = false;
            for (const auto& tile : tiles) {
                if (checkCollision(bullet.rect, tile.rect)) { bullet.active = false; break; }
            }
            if (bullet.rect.x < cameraX || bullet.rect.x > cameraX + SCREEN_WIDTH) bullet.active = false;
        }
    }

    updateEnemies();

    for (auto& spike : spikes) {
        spike.x = (int)cameraX;
        if (!isInvincible &&
            playerRect.x <= spike.x + spike.w &&
            playerRect.x + playerRect.w > spike.x &&
            playerRect.y + playerRect.h > spike.y &&
            playerRect.y < spike.y + spike.h) {
            lives--;
            playSFX(hitSound);
            heartTextures[lives] = heartTextures[3];
            invincibilityTimer = SDL_GetTicks() + 2000;
            isInvincible = true;
            if (lives <= 0) {
                if (score > bestScore) { bestScore = score; saveBestScore(); }
                gameState = GAME_OVER;
            }
            break;
        }
    }

    if (playerRect.y > SCREEN_HEIGHT) {
        if (lives > 0) {
            lives--;
            playSFX(hitSound);
            heartTextures[lives] = heartTextures[3];
            playerRect.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
            playerVelY = JUMP_FORCE;
            invincibilityTimer = SDL_GetTicks() + 2000;
            isInvincible = true;
        } else {
            if (score > bestScore) { bestScore = score; saveBestScore(); }
            gameState = GAME_OVER;
        }
    }

    if (isInvincible && SDL_GetTicks() > invincibilityTimer) isInvincible = false;

    cleanUpObjects();
    if (tiles.empty() || tiles.back().rect.x + tiles.back().rect.w < cameraX + SCREEN_WIDTH + TILE_SIZE * 10) generateWorld();
}

void Game::render() {
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {
        for (int x = 0; x < SCREEN_WIDTH + (int)cameraX; x += TILE_SIZE) {
            SDL_Rect dest = {x - (int)cameraX, y, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &dest);
        }
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color black = {87, 22, 112, 255};

    if (gameState == MAIN_MENU) {
        renderText("Umbraked", SCREEN_WIDTH / 2, 80, yellow, titleFont, true);
        menuButtons = {{{SCREEN_WIDTH / 2 - 25, 250, 100, 50}, "Play"},
                       {{SCREEN_WIDTH / 2 - 25, 320, 100, 50}, "Instructions"},
                       {{SCREEN_WIDTH / 2 - 25, 460, 100, 50}, "Records"},
                       {{SCREEN_WIDTH / 2 - 25, 390, 100, 50}, "Options"}};
        for (const auto& button : menuButtons) {
            renderText(button.text.c_str(), button.rect.x + 25, button.rect.y + 10, white, font, true);
        }
    } else if (gameState == INSTRUCTIONS) {
        renderText("Instructions", SCREEN_WIDTH / 2, 100, white, font, true);
        renderText("A/D to move", SCREEN_WIDTH / 2, 200, white, font, true);
        renderText("W to jump", SCREEN_WIDTH / 2, 250, white, font, true);
        renderText("SPACE to shoot", SCREEN_WIDTH / 2, 300, white, font, true);
        renderText("(Switch UNIKEY to E mode)", SCREEN_WIDTH / 2, 350, white, font, true);
        menuButtons = {{{SCREEN_WIDTH / 2 - 25, 450, 100, 50}, "Back"}};
        renderText("Back", menuButtons[0].rect.x + 25, menuButtons[0].rect.y + 10, white, font, true);
    } else if (gameState == RECORDS) {
        renderText("Records", SCREEN_WIDTH / 2, 100, white, font, true);
        std::string bestScoreText = "Best Score: " + std::to_string(bestScore);
        renderText(bestScoreText.c_str(), SCREEN_WIDTH / 2, 200, white, font, true);
        menuButtons = {{{SCREEN_WIDTH / 2 - 30, 300, 100, 50}, "Back"}};
        renderText("Back", menuButtons[0].rect.x + 25, menuButtons[0].rect.y + 10, white, font, true);
    } else if (gameState == OPTIONS) {
        renderText("Options", SCREEN_WIDTH / 2, 100, white, font, true);
        menuButtons = {{{SCREEN_WIDTH / 2 + 50, 200, 50, 50}, musicOn ? "ON" : "OFF"},
                       {{SCREEN_WIDTH / 2 + 50, 270, 50, 50}, sfxOn ? "ON" : "OFF"},
                       {{SCREEN_WIDTH / 2 - 25, 340, 100, 50}, "Back"}};
        renderText("Music:", SCREEN_WIDTH / 2 - 100, 210, white, font, false);
        renderText(musicOn ? "ON" : "OFF", menuButtons[0].rect.x, menuButtons[0].rect.y + 10, white, font, false);
        renderText("SFX:", SCREEN_WIDTH / 2 - 100, 280, white, font, false);
        renderText(sfxOn ? "ON" : "OFF", menuButtons[1].rect.x, menuButtons[1].rect.y + 10, white, font, false);
        renderText("Back", menuButtons[2].rect.x + 25, menuButtons[2].rect.y + 10, white, font, true);
    } else if (gameState == PLAYING) {
        for (const auto& tile : tiles) {
            SDL_Rect dest = {tile.rect.x - (int)cameraX, tile.rect.y, tile.rect.w, tile.rect.h};
            SDL_RenderCopy(renderer, tile.isGround ? groundTexture : floatingTexture, NULL, &dest);
        }
        for (const auto& spike : spikes) {
            SDL_Rect dest = {spike.x - (int)cameraX, spike.y, spike.w, spike.h};
            SDL_RenderCopy(renderer, spikeTexture, NULL, &dest);
        }
        for (const auto& enemy : enemies) {
            if (enemy.active) {
                SDL_Rect dest = {enemy.rect.x - (int)cameraX, enemy.rect.y, enemy.rect.w, enemy.rect.h};
                SDL_RendererFlip flip = enemy.facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                switch (enemy.type) {
                    case 0: SDL_RenderCopyEx(renderer, enemy1Texture, NULL, &dest, 0, NULL, flip); break;
                    case 1: SDL_RenderCopyEx(renderer, enemy2Texture, NULL, &dest, 0, NULL, flip); break;
                    case 2: SDL_RenderCopyEx(renderer, enemy3Texture, NULL, &dest, 0, NULL, flip); break;
                    case 3: SDL_RenderCopyEx(renderer, enemy4Texture, NULL, &dest, 0, NULL, flip); break;
                    case 4: SDL_RenderCopyEx(renderer, enemy5Texture, NULL, &dest, 0, NULL, flip); break;
                }
            }
        }
        for (const auto& bullet : bullets) {
            if (bullet.active) {
                SDL_Rect dest = {bullet.rect.x - (int)cameraX, bullet.rect.y, bullet.rect.w, bullet.rect.h};
                SDL_RenderCopy(renderer, bulletTexture, NULL, &dest);
            }
        }
        for (const auto& bullet : enemyBullets) {
            if (bullet.active) {
                SDL_Rect dest = {bullet.rect.x - (int)cameraX, bullet.rect.y, bullet.rect.w, bullet.rect.h};
                SDL_RenderCopy(renderer, enemyBulletTexture, NULL, &dest);
            }
        }
        SDL_Rect playerDest = {playerRect.x - (int)cameraX, playerRect.y, playerRect.w, playerRect.h};
        SDL_RendererFlip flip = playerFlipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        if (!isInvincible || (SDL_GetTicks() % 200 < 100)) {
            SDL_RenderCopyEx(renderer, playerTexture, NULL, &playerDest, 0, NULL, flip);
        }
        for (int i = 0; i < 3; i++) {
            SDL_Rect heartRect = {10 + i * 40, 10, 32, 32};
            SDL_RenderCopy(renderer, heartTextures[i], NULL, &heartRect);
        }
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText.c_str(), SCREEN_WIDTH - 400, 10, black, scoreFont);
    } else if (gameState == GAME_OVER) {
        renderText("GAME OVER", SCREEN_WIDTH / 2, 100, white, font, true);
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText.c_str(), SCREEN_WIDTH / 2, 150, white, font, true);
        menuButtons = {{{SCREEN_WIDTH / 2 - 25, 270, 100, 50}, "Play Again"},
                       {{SCREEN_WIDTH / 2 - 25, 340, 100, 50}, "Main Menu"},
                       {{SCREEN_WIDTH / 2 - 25, 410, 100, 50}, "Exit"}};
        for (const auto& button : menuButtons) {
            renderText(button.text.c_str(), button.rect.x + 25, button.rect.y + 10, white, font, true);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::renderText(const char* text, int x, int y, SDL_Color color, TTF_Font* font, bool center) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) { SDL_FreeSurface(surface); return; }
    SDL_Rect rect = {x, y, surface->w, surface->h};
    if (center) rect.x -= surface->w / 2;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::generateWorld() {
<<<<<<< HEAD
    static float spawnThreshold = 0.9f;
=======
    static float spawnThreshold = 0.9f; // Ngưỡng spawn ban đầu
>>>>>>> 1aaf4da6794a26c504f0406884b8b85a90d74dff

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

    if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold) {
        int spawnX1 = lastGeneratedX + segmentLength / 3;
        int spawnX2 = lastGeneratedX + segmentLength * 2 / 3;
        if (std::abs(spawnX1 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
            spawnEnemy(spawnX1, groundHeight - 40);
        }
        if (spawnDist(gen) < spawnThreshold * 0.6f && std::abs(spawnX2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
            spawnEnemy(spawnX2, groundHeight - 40);
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

                if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold * 0.8f && numPlatforms > 1 &&
                    std::abs(platformX + platformWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                    spawnEnemy(platformX + platformWidth / 2, platformY - 40);
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
        if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold * 0.8f &&
            std::abs(lastGeneratedX + pipeWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
            spawnEnemy(lastGeneratedX + pipeWidth / 2, groundHeight - pipeHeight - 40);
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
            if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold * 0.8f && numPlatforms > 1 &&
                std::abs(platformX + platformWidth / 2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                spawnEnemy(platformX + platformWidth / 2, platformY - 40);
                if (platformWidth > TILE_SIZE && spawnDist(gen) < spawnThreshold * 0.4f) {
                    spawnEnemy(platformX, platformY - 40);
                }
            }
            prevX = platformX + platformWidth;
            prevY = platformY;
        }
        lastGeneratedX += totalWidth + TILE_SIZE * 2;
    } else {
        if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold) {
            int spawnX = lastGeneratedX + TILE_SIZE;
            if (std::abs(spawnX - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                spawnEnemy(spawnX, groundHeight - 40);
            }
            if (spawnDist(gen) < spawnThreshold * 0.5f) {
                int spawnX2 = lastGeneratedX + TILE_SIZE * 2;
                if (std::abs(spawnX2 - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                    spawnEnemy(spawnX2, groundHeight - 40);
                }
            }
        }
        lastGeneratedX += TILE_SIZE * 3;
    }
    if (lastGeneratedX > SCREEN_WIDTH && spawnDist(gen) < spawnThreshold * 0.7f) {
        int soloBlockX = lastGeneratedX - segmentLength / 2;
        int soloBlockY = groundHeight - TILE_SIZE * (rand() % 4 + 1);
        if (soloBlockY < groundHeight - 4 * TILE_SIZE) {
            tiles.push_back({{soloBlockX, soloBlockY, TILE_SIZE, TILE_SIZE}, false});
            if (spawnDist(gen) < spawnThreshold * 0.6f && std::abs(soloBlockX - playerRect.x) > MIN_ENEMY_SPAWN_DISTANCE) {
                spawnEnemy(soloBlockX, soloBlockY - 40);
            }
        }
    }
}

void Game::resetGame() {
    isJumping = false;
    isOnGround = true;
    playerVelX = 0;
    playerVelY = 0;
    score = 0;
    cameraX = 0;
    maxPlayerX = 0;
    shootCooldown = 0;
    groundHeight = GROUND_HEIGHT;
    lastGeneratedX = 0;
    lives = 3;
    invincibilityTimer = SDL_GetTicks() + 2000;
    isInvincible = true;
    tiles.clear();
    bullets.clear();
    enemyBullets.clear();
    enemies.clear();

    if (spikes.empty()) {
        for (int y = 0; y < SCREEN_HEIGHT; y += TILE_SIZE) {
            spikes.push_back({0, y, TILE_SIZE, TILE_SIZE});
        }
    } else {
        for (auto& spike : spikes) spike.x = 0;
    }

    SDL_Surface* surface = IMG_Load(LIVE_IMAGE_PATH);
    SDL_Texture* liveTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    heartTextures[0] = liveTexture;
    heartTextures[1] = liveTexture;
    heartTextures[2] = liveTexture;

    for (int i = 0; i < 30; i++) generateWorld();

    playerRect = {cameraX + TILE_SIZE + 50, GROUND_HEIGHT - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};
}

void Game::fireBullet() {
    bullets.push_back({{playerRect.x + (playerFlipped ? 0 : playerRect.w), playerRect.y + playerRect.h / 2 - 2, 10, 5},
                       10.0f, true, playerFlipped, 0, playerRect.x + (playerFlipped ? 0 : playerRect.w)});
    playSFX(shootSound);
}

void Game::spawnEnemy(int x, int y) {
<<<<<<< HEAD
    static float spawnThreshold = 0.9f;
=======
    static float spawnThreshold = 0.9f; // Ngưỡng spawn ban đầu
>>>>>>> 1aaf4da6794a26c504f0406884b8b85a90d74dff
    if (spawnDist(gen) >= spawnThreshold) return;

    Enemy enemy;
    enemy.type = rand() % 5;
    int baseHeight = (enemy.type == 3) ? 40 : 30;
    int width = (enemy.type == 3) ? static_cast<int>(baseHeight * enemy4AspectRatio) :
                (enemy.type == 4) ? static_cast<int>(baseHeight * enemy5AspectRatio) : 30;

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

bool Game::canSpawnEnemy(int x, int y, int width, int height) {
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

void Game::updateEnemies() {
<<<<<<< HEAD
    static float enemyBulletSpeed = 3.0f;
=======
    static float enemyBulletSpeed = 3.0f; // Tốc độ đạn enemy ban đầu
>>>>>>> 1aaf4da6794a26c504f0406884b8b85a90d74dff

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
                                        enemyBulletSpeed + (enemy.type <= 1 ? 0 : 1.0f), true, enemy.facingLeft, 0,
                                        enemy.rect.x + (enemy.facingLeft ? 0 : enemy.rect.w)});
                enemy.shootCooldown = (enemy.type <= 1) ? 60 : 45;
            } else if (enemy.shootCooldown > 0) {
                enemy.shootCooldown--;
            }
        }

        for (auto& bullet : bullets) {
            if (bullet.active && checkCollision(bullet.rect, enemy.rect)) {
                bullet.active = false;
                enemy.active = false;
                playSFX(boomSound);
                score += (enemy.type <= 1) ? 100 : 150;
                break;
            }
        }

        if (checkCollision(playerRect, enemy.rect) && !isInvincible) {
            if (playerRect.y + playerRect.h < enemy.rect.y + enemy.rect.h / 2 && playerVelY > 0) {
                enemy.active = false;
                playerVelY = JUMP_FORCE / 2;
                playSFX(boomSound);
                score += 50;
            } else {
                lives--;
                playSFX(hitSound);
                heartTextures[lives] = heartTextures[3];
                invincibilityTimer = SDL_GetTicks() + 2000;
                isInvincible = true;
                if (lives <= 0) {
                    if (score > bestScore) { bestScore = score; saveBestScore(); }
                    gameState = GAME_OVER;
                }
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
            if (bullet.rect.x < cameraX || bullet.rect.x > cameraX + SCREEN_WIDTH) bullet.active = false;
            if (checkCollision(playerRect, bullet.rect) && !isInvincible) {
                bullet.active = false;
                lives--;
                playSFX(hitSound);
                heartTextures[lives] = heartTextures[3];
                invincibilityTimer = SDL_GetTicks() + 2000;
                isInvincible = true;
                if (lives <= 0) {
                    if (score > bestScore) { bestScore = score; saveBestScore(); }
                    gameState = GAME_OVER;
                }
            }
        }
    }
}

void Game::cleanUpObjects() {
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(),
        [this](const Tile& tile) { return tile.rect.x + tile.rect.w < cameraX; }), tiles.end());
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
    enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
        [](const Bullet& b) { return !b.active; }), enemyBullets.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return !e.active; }), enemies.end());
}

void Game::loadBestScore() {
    std::ifstream file("best_score.txt");
    if (file.is_open()) { file >> bestScore; file.close(); }
}

void Game::saveBestScore() {
    std::ofstream file("best_score.txt");
    if (file.is_open()) { file << bestScore; file.close(); }
}

void Game::updateMusic() {
    if (gameState == MAIN_MENU || gameState == INSTRUCTIONS || gameState == RECORDS || gameState == OPTIONS) {
        if (musicOn && !Mix_PlayingMusic()) Mix_PlayMusic(menuMusic, -1);
        else if (!musicOn && Mix_PlayingMusic()) Mix_HaltMusic();
    } else if (gameState == PLAYING) {
        if (musicOn && !Mix_PlayingMusic()) Mix_PlayMusic(inGameMusic, -1);
        else if (!musicOn && Mix_PlayingMusic()) Mix_HaltMusic();
    } else if (gameState == GAME_OVER) Mix_HaltMusic();
}

void Game::playSFX(Mix_Chunk* sound) {
    if (sfxOn) Mix_PlayChannel(-1, sound, 0);
}

void Game::close() {
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
    TTF_CloseFont(font);
    TTF_CloseFont(titleFont);
    TTF_CloseFont(scoreFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(boomSound);
    Mix_FreeChunk(jumpSound);
    Mix_FreeMusic(inGameMusic);
    Mix_FreeMusic(menuMusic);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int Game::getNextDifficultyThreshold(int currentScore) {
    if (currentScore < 100) return 100;

    int temp = currentScore;
    int digits = 0;
    while (temp > 0) {
        temp /= 10;
        digits++;
    }

    int base = 1;
    for (int i = 1; i < digits; i++) {
        base *= 10;
    }
    int firstDigit = (currentScore / base) + 1;
    return firstDigit * base;
}
