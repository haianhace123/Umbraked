#ifndef CONFIG_H
#define CONFIG_H

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int TILE_SIZE = 32;
constexpr int PLAYER_WIDTH = 30;
constexpr int PLAYER_HEIGHT = 45;
constexpr float JUMP_FORCE = -14.0f;
constexpr float GRAVITY = 0.5f;
constexpr float PLAYER_SPEED = 5.0f;
constexpr float CAMERA_SPEED = 2.0f;
constexpr int FONT_SIZE = 48;
constexpr const char* TITLE_FONT_PATH = "txt/Purisa-BoldOblique.ttf";
constexpr const char* FONT_PATH = "txt/SVN-Coder's Crux.ttf";
constexpr const char* PLAYER_IMAGE_PATH = "img/nvc.png";
constexpr const char* GROUND_TILE_PATH = "img/platform.png";
constexpr const char* FLOATING_TILE_PATH = "img/platform.png";
constexpr const char* ENEMY1_IMAGE_PATH = "img/enemy1.png";
constexpr const char* ENEMY2_IMAGE_PATH = "img/enemy2.png";
constexpr const char* ENEMY3_IMAGE_PATH = "img/enemy3.png";
constexpr const char* ENEMY4_IMAGE_PATH = "img/enemy4.png";
constexpr const char* ENEMY5_IMAGE_PATH = "img/enemy5.png";
constexpr const char* BULLET_IMAGE_PATH = "img/Bullet.png";
constexpr const char* ENEMY_BULLET_IMAGE_PATH = "img/enemybullet.png";
constexpr const char* BACKGROUND_IMAGE_PATH = "img/background.png";
constexpr const char* LIVE_IMAGE_PATH = "img/live.png";
constexpr const char* DIE_IMAGE_PATH = "img/die.png";
constexpr const char* SPIKE_IMAGE_PATH = "img/gai.png";
constexpr const char* HIT_SOUND_PATH = "sound/rung.wav";
constexpr const char* SHOOT_SOUND_PATH = "sound/shoot.wav";
constexpr const char* BOOM_SOUND_PATH = "sound/boom.wav";
constexpr const char* JUMP_SOUND_PATH = "sound/jump.wav";
constexpr const char* INGAME_SOUND_PATH = "sound/soundingame.wav";
constexpr const char* MENU_SOUND_PATH = "sound/soundmenu.wav";
constexpr int MAX_JUMP_DISTANCE = TILE_SIZE * 4;
constexpr int GROUND_HEIGHT = SCREEN_HEIGHT - TILE_SIZE * 3;
constexpr int MIN_ENEMY_SPAWN_DISTANCE = 300;
constexpr int BULLET_MAX_DISTANCE = 300;

enum GameState { MAIN_MENU, INSTRUCTIONS, RECORDS, OPTIONS, PLAYING, GAME_OVER };

#endif
