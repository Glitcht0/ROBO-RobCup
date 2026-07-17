#pragma once
#include "raylib.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#define MAX_LIGHTS 4
#define LARGURA_TELA 1280
#define ALTURA_TELA 720


#define BASE_W 1280.0f
#define BASE_H 720.0f

extern float EXEMPLO_VARIAVEL;


//#define LARGURA_TELA 1920
//#define ALTURA_TELA 1080


static const int WORLD_W = 200;
static const int WORLD_H = 200;

static const int CHUNK_SIZE = 16;
static const int TILE_CHUNK_SIZE = 16;
static const int VIEW_DISTANCE = 5;




enum class GameMode {
    EDIT,
    MOVE,
    GAME,
    ROTATE
};

#define COR_MESA (Color){ 45, 45, 45, 255 }
#define COR_INPUT (Color){ 84, 84, 84, 255 }
#define COR_GRAMA (Color){ 244, 216, 149, 255 }