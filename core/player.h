#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "../common.h"

Player* getPlayer();
ListManager* getPlayerList();
Player* genPlayer(char* name);
Player* initPlayer(Player* p);

void resetPlayersBomb();
void placeBomb(Player* p);

void* updatePlayers();
void updatePlayerClip(Player* p);
void* playerMove(Player* p, short direction);
void calcPlayerObjectPos(SDL_Rect playPos, SDL_Rect* objPos);

void clearPlayers();
void* deletePlayer(Player* p);


#endif