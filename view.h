#ifndef _VIEW_H__
#define _VIEW_H__

#include "common.h"
#include "animation.h"

typedef struct BtnAnim BtnAnim;
struct BtnAnim {
	int x;
	int y;
	int time;
	int delay;
};


struct Button {
	short z;
	char* name;
	char* text;
	SDL_Rect pos;
	char* imgPath;
	char* imgHoverPath;

	Object* imgObj;
	Object* txtObj;

	void* (*click)(Object* obj);
	void* (*hover)(Object* obj);

	AnimParam* anim;

	short hasAnim;
	
	int x;
	int y;

	float time;
	float delay;
};


SDL_Surface* getScreen();
ListManager* getLayers();
short setObjectLayer(Object* obj, short z);
short isHovered(Object* obj, int x, int y);
Object* getClicked(int x, int y, short hover);
void render();
void buttonUnHover();
void buttonHover(Object* obj);
void deleteBtn(Button* btn);
Object** getHovered();

#endif