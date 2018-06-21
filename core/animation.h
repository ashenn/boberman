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
	struct AnimDistance xDist;
	struct AnimDistance yDist;

	short init;
	short deleteObject;
	
	void* (*fnc)();
	short (*custFnc)();
	void (*callBack	)(struct AnimParam*);
	
	struct Object* obj;
	SDL_Rect target;
	short boolean;
};

void animate();
struct AnimParam* moveTo(struct Object* obj, int x, int y, float time, float delay);
void animRemoveObject(struct Object* obj);
struct AnimParam* spriteAnim(struct Object* obj, SDL_Rect clip, float time, float delay, short applyToChilds);
struct AnimParam* customAnim(struct Object* obj, float loopTime, float delay, short (*fnc) (struct AnimParam*));

#endif