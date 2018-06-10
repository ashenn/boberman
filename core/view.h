#ifndef _VIEW_H__
#define _VIEW_H__

#include "../common.h"
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
	char* font;
	
	short fontSize;

	SDL_Rect pos;
	SDL_Color color;
	
	char* imgPath;
	char* imgHoverPath;

	Object* imgObj;
	Object* txtObj;


	void* (*click)(Object* obj);
	void* (*hover)(Object* obj);

	short hasAnim;
	AnimParam* anim;
};


void* render(void* arg);
SDL_Surface* getScreen();
ListManager* getLayers();
short setObjectLayer(Object* obj, short z);
short isHovered(Object* obj, int x, int y);
Object* getClicked(int x, int y, short hover);
void buttonUnHover(Object* obj);
void buttonHover(Object* obj);
void deleteBtn(Button* btn);
Object** getHovered();
void clearScreen();

#endif