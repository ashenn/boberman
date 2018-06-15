#ifndef _OBJECT_H__
#define _OBJECT_H__

#include "../common.h"

enum ObjContType
{
	NONE,
	BOMB,
	BLOCK,
	BONUS,
	BUTTON,
	PLAYER
};

/**
 * Visual Object
 */
struct Object {
	int id;
	int layerId;
	short z;	// z-index
	char* name;
	short lifetime;
	
	short visible;	// Displayed
	short enabled;	// Clickable / Hoverable
	
	int* clipIndex;
	SDL_Rect pos;	// Poition / site
	SDL_Rect* clip;	// Image Clip
	
	Collision* collision;

	void* container;	// Custom Struc ex: Button
	ObjContType containerType;

	Object* parent;
	ListManager* childs;	// Linked Objects, will be diplayed / animated with parent

	unsigned int color;
	SDL_Surface* component;	// SDL Visual

	void* (*click)(Object* obj);	// Click CallBack
	void* (*hover)(Object* obj);	// Hover Callback

	void (*onDelete)(Object* obj);	// Hover Callback
	//pthread_cond_t threadCond;
	//pthread_mutex_t threadMutex;
};


ListManager* getObjectList();	// List Of all visual Objects
Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z); // Add Object No Click / Hover / Container
Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container);

void clearObjects();
void deleteObject(Object* obj);
short layerSort(void* a, void* b);
Object* generateButton(Button* btn);
short addChild(Object* obj, Object* child);
Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z);

void clearOutdatedObjects();
#endif