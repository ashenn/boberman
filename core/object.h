#ifndef _OBJECT_H__
#define _OBJECT_H__

#include "../common.h"

/**
 * Visual Object
 */
struct Object {
	int id;
	short z;	// z-index
	char* name;
	SDL_Rect pos;	// Poition / site
	short visible;	// Displayed
	short enabled;	// Clickable / Hoverable

	void* container;	// Custom Struc ex: Button
	Object* childObj;	// Linked Object, will be diplayed / animated with parent

	SDL_Surface* component;	// SDL Visual

	void* (*click)(Object* obj);	// Click CallBack
	void* (*hover)(Object* obj);	// Hover Callback
};


struct Player {
	int id;
	short z;
	char* name;

	SDL_Rect pos;

	char* imgPath;
	Object* imgObj;

	SDL_Surface* sprite;
	SDL_Surface* component;
};


ListManager* getObjectList();	// List Of all visual Objects
Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z); // Add Object No Click / Hover / Container
Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container);

void deleteObject(Object* obj);
Object* generateButton(Button* btn);

#endif