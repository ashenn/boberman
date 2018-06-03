#ifndef _OBJECT_H__
#define _OBJECT_H__

#include "../common.h"

struct Collision {
	SDL_Rect pos;
	short enabled;

	void* (*fnc)(Object* obj, Object* target);	// Hit
};

/**
 * Visual Object
 */
struct Object {
	int id;
	int layerId;
	short z;	// z-index
	char* name;
	
	short visible;	// Displayed
	short enabled;	// Clickable / Hoverable
	
	int* clipIndex;
	SDL_Rect pos;	// Poition / site
	SDL_Rect* clip;	// Image Clip
	
	SDL_Rect hitbox;

	void* container;	// Custom Struc ex: Button
	ObjContType containerType; //
	ListManager* childs;	// Linked Objects, will be diplayed / animated with parent
	Object* hitObj;

	unsigned int color;

	SDL_Surface* component;	// SDL Visual

	void* (*click)(Object* obj);	// Click CallBack
	void* (*hover)(Object* obj);	// Hover Callback
	void (*hit)(Object* obj, Object* target);	// Collision Callback
};


ListManager* getObjectList();	// List Of all visual Objects
Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z); // Add Object No Click / Hover / Container
Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container);

void deleteObject(Object* obj);
Object* generateButton(Button* btn);
short layerSort(void* a, void* b);
void clearObjects();
Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z);
short addChild(Object* obj, Object* child);
void setHitBox(Object* obj, SDL_Rect rect);
void setHitBox(Object* obj, SDL_Rect rect);
void handleHits();
ListManager* getHitObjectList();
#endif