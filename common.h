#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <pthread.h>


#include "base/basic.h"
#include "base/libList.h"
#include "base/libParse.h"
#include "base/logger.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#define CELL_SIZE 55

#define SCREEN_W 800
#define SCREEN_H 600

#define FONT_XS 16
#define FONT_SM 20
#define FONT_MD 24
#define FONT_LG 28

#define LG_BTN_H 45
#define LG_BTN_W 200

#define SM_BTN_H 35
#define SM_BTN_W 150

#define ICO_BTN_H 20
#define ICO_BTN_W 20

#define true 1
#define false 0

#define FPS 45

#define PLAYER_W 55
#define PLAYER_H 83

#define PLAYER_ANIM_LEN 3

#define MAP_X 42
#define MAP_Y 76

#define MAP_H 9
#define MAP_W 13

#define BOMB_SIZE 36
#define BONUS_SIZE 36

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

#define WALK_FRAMES 2
#define TICK_FRAMES 5
#define TICK_REFRESH 4

#define MSG_SIZE 500

#include "core/asset.h"

typedef enum ObjContType ObjContType;

typedef struct Button Button;
typedef struct Collision Collision;
typedef struct Object Object;

typedef struct Animator Animator;
typedef struct AnimParam AnimParam;
typedef struct AnimDistance AnimDistance;
typedef enum ExplosionPart ExplosionPart;

typedef struct Player Player;
struct Player {
	int id;
	char* name;
	short alive;
	short direction;

	float speed;
	short shoot;
	short bombCnt;
	short bombMax;
	short bombPower;
	short canPlaceBomb;

	int clipIndex;

	SDL_Rect pos;
	SDL_Rect clip;

	Object* object;
};

typedef struct Bomb Bomb;
struct Bomb {
	int id;
	short z;	// z-index
	short state;
	short power;
	char* name;
	short exploded;
	short direction;

	short moveIterate;

	Player* owner;

	SDL_Rect clip;
	int* clipIndex;

	Object* obj;
	SDL_Rect pos;
};

void killPlayer(Player* p);

#include "core/player.h"
#include "core/bomb.h"
#include "core/event.h"
#include "main.h"
#include "core/map.h"
#include "core/collision.h"
#include "core/object.h"
#include "core/game.h"
#include "core/view.h"
#include "base/math.h"
#include "core/animation.h"
#include "core/bonus.h"

#endif
