#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "../common.h"

#define COL_NONE 0
#define COL_WALL 1
#define COL_BOMB 2
#define COL_BONUS 4
#define COL_PLAYER 8
#define COL_ALL 16

struct Collision {
	SDL_Rect pos;
	int id;

	short flag;
	short colFlags;

	short enabled;
	short deleteOnCol;
	short blocking;

	Object* dbgObj;

	void (*fnc)(Object* obj, Object* target); // Hit
};


void handleHits();
ListManager* getHitObjectList();
short canMoveTo(Object* obj, int x, int y);
void prepareCol(Object* obj, Collision* col);
short doesObjectCollides(Object* o, Object* o2);
short doesCollide(Collision* col, Collision* col2);
void prepareColAtPos(Object* obj, Collision* col, int x, int y);
void setHitBox(Object* obj, SDL_Rect rect, short blocking, int flag);
Object* getObjectByCollision(Collision* col2, short blocking, Object* ignore);

#endif