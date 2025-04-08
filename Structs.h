#ifndef STRUCTS_H
#define STRUCTS_H
#include <SDL.h>
#include <string>

struct Tile {
    SDL_Rect rect;
    bool isGround;
};

struct Button {
    SDL_Rect rect;
    std::string text;
};

struct Bullet {
    SDL_Rect rect;
    float speed;
    bool active;
    bool facingLeft;
    int distanceTraveled;
    int startX;
};

struct Enemy {
    SDL_Rect rect;
    float speed;
    bool active;
    bool facingLeft;
    int type;
    int shootCooldown;
    float detectionRange;
    float velocityY;
};

#endif
