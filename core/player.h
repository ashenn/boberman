#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "../common.h"


typedef struct Bomb Bomb;
struct Bomb {
	int id;
	short z;	// z-index
	short power;
	short state;
	char* name;
	
	SDL_Rect clip;
	int* clipIndex;

	Object* obj;
	SDL_Rect pos;
};

typedef struct Player Player;
struct Player {
	int id;
	char* name;
	short alive;

	int clipIndex;

	SDL_Rect pos;
	SDL_Rect clip;
	short direction;

	Object* object;

	SDL_Surface* sprite;
	SDL_Surface* component;
};

ListManager* getPlayerList();
void clearPlayers();
void* deletePlayer(Player* p);
Player* genPlayer(char* name);
void calcPlayerObjectPos(SDL_Rect playPos, SDL_Rect* objPos);
void* updatePlayers();
Player* initPlayer(Player* p);
Player* getPlayer();
void playerMove(Player* p, short direction);
void updatePlayerClip(Player* p);

void placeBomb(Player* p );

#endif