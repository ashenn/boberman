#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "../common.h"



struct Player {
	int id;
	char* name;

	SDL_Rect pos;

	char* imgPath;
	Object* imgObj;

	SDL_Surface* sprite;
	SDL_Surface* component;
};

#endif