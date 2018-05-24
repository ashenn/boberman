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

#define SCREEN_H 600
#define SCREEN_W 800

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

#define GAME_MENU 0
#define GAME_LOBY 1
#define GAME_READY 2
#define GAME_SART 3
#define GAME_RUNNING 4
#define GAME_END 5
#define GAME_QUIT 6

#define FPS 60

typedef struct Game Game;
struct Game {
	short status;
};

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
	void* (*fnc)();
	
	Object* obj;
	SDL_Rect* target;
};

#include "core/object.h"
#include "core/view.h"

#include "base/math.h"
#include "core/animation.h"

#endif