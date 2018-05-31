#ifndef _ANIMATION_H__
#define _ANIMATION_H__

#include "../common.h"


typedef struct Animator Animator;
struct Animator {
	ListManager* objects;
	ListManager* sprites;
};

void animate();
AnimParam* moveTo(Object* obj, int x, int y, float time, float delay);
void animRemoveObject(Object* obj);

#endif