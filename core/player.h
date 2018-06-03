#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "../common.h"


typedef enum ExplosionPart ExplosionPart;
enum ExplosionPart
{
	EXPL_CENTER,
	EXPL_MID_H,
	EXPL_MID_V,
	EXPL_END_TOP,
	EXPL_END_DOWN,
	EXPL_END_LEFT,
	EXPL_END_RIGHT,
};

typedef struct Bomb Bomb;
struct Bomb {
	int id;
	short z;	// z-index
	short state;
	short power;
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
	short bombPower;

	int clipIndex;

	SDL_Rect pos;
	SDL_Rect clip;
	short direction;

	Object* object;
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
void killPlayer(Player* p);

#endif