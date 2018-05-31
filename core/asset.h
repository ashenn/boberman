#ifndef _ASSET_H__
#define _ASSET_H__

#include "../common.h"

typedef struct AssetMgr AssetMgr;
struct AssetMgr {
	ListManager* imgs;
	ListManager* fonts;

	void* (*getImg) (char* name);
	void* (*getFont) (char* name, short size);

	void (*clear) ();
	void (*clearImgs) ();
	void (*clearFonts) ();

	void* (*isCached) (char* name, ListManager* cont);
	void* (*cache) (char* name, ListManager* cont, void* data);
	
	void (*destroy) ();
};

AssetMgr* getAssets();

#endif