#ifndef _PLAYER_H__
#define _PLAYER_H__

#include "../common.h"

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

void placeBomb(Player* p);
void placeExplosion(Bomb* bomb);
void clearExplosion(Object* obj);
void iterateBomb(AnimParam* anim);
void iterateExplosion(AnimParam* anim);
void getExplPartName(ExplosionPart part, char* name);

#endif