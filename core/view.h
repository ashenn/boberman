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

	struct Object* imgObj;
	struct Object* txtObj;


	void* (*click)(struct Object* obj);
	void* (*hover)(struct Object* obj);

	short hasAnim;
	struct AnimParam* anim;
};


void* render(void* arg);
SDL_Surface* getScreen();
ListManager* getLayers();
short setObjectLayer(struct Object* obj, short z);
short isHovered(struct Object* obj, int x, int y);
struct Object* getClicked(int x, int y, short hover);
void buttonUnHover(struct Object* obj);
void buttonHover(struct Object* obj);
void deleteBtn(struct Button* btn);
struct Object** getHovered();
void clearScreen();

#endif