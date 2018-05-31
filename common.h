#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <time.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

#define FPS 60

#define PLAYER_W 55
#define PLAYER_H 83

#define PLAYER_ANIM_LEN 3

#define MAP_X 42
#define MAP_Y 76

#define MAP_SIZE_X 13
#define MAP_SIZE_Y 9

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

#define WALK_FRAMES 2

#include "core/asset.h"

typedef struct Button Button;
typedef struct Object Object;

typedef struct AnimDistance AnimDistance;
struct AnimDistance {
	int perFrame;
	int rest;
};


typedef struct AnimParam AnimParam;
struct AnimParam {
	int id;
	float time;
	float delay;
	
	SDL_Rect pos;

	int frames;
	AnimDistance xDist;
	AnimDistance yDist;

	short init;
	short deleteObject;
	void* (*fnc)();
	void (*callBack	)(AnimParam*);
	
	Object* obj;
	SDL_Rect* target;
};

#include "core/player.h"
#include "core/event.h"
#include "main.h"
#include "core/game.h"
#include "core/object.h"
#include "core/view.h"

#include "base/math.h"
#include "core/animation.h"

#endif