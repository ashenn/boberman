#ifndef _ANIMATION_H__
#define _ANIMATION_H__

#include "../common.h"

struct Animator {
	ListManager* objects;
	ListManager* sprites;
};

struct AnimDistance {
	int perFrame;
	int rest;
};

struct AnimParam {
	int id;
	float time;
	float delay;
	
	SDL_Rect pos;

	int frames;
	int breakAnim;
	AnimDistance xDist;
	AnimDistance yDist;

	short init;
	short deleteObject;
	
	void* (*fnc)();
	short (*custFnc)();
	void (*callBack	)(AnimParam*);
	
	Object* obj;
	SDL_Rect target;
	short boolean;
};

void animate();
AnimParam* moveTo(Object* obj, int x, int y, float time, float delay);
void animRemoveObject(Object* obj);
AnimParam* spriteAnim(Object* obj, SDL_Rect clip, float time, float delay, short applyToChilds);
AnimParam* customAnim(Object* obj, float loopTime, float delay, short (*fnc) (AnimParam*));

#endif