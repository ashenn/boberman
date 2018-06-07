#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "../common.h"

struct Collision {
	SDL_Rect pos;
	int id;

	short enabled;
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
void setHitBox(Object* obj, SDL_Rect rect, short blocking);
void prepareColAtPos(Object* obj, Collision* col, int x, int y);
Object* getObjectByCollision(Collision* col2, short blocking, Object* ignore);

#endif