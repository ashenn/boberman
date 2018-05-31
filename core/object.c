#include "object.h"

void clearObjects() {
	Node* n = NULL;
	logger->dbg("-- Getting Objects");
	ListManager* objects = getObjectList();

	logger->dbg("-- Cleaning Hover");
	Object** curHover = getHovered();
	*curHover = NULL;

	if (objects != NULL){
		logger->dbg("-- Len: %d", objects->nodeCount);

		while((n = listIterate(objects, n)) != NULL) {
			logger->dbg("-- Deleting");
			logger->dbg("-- Deleting: %s", n->name);
		    deleteObject(n->value);
		    n = NULL;
		}
	}

	clearScreen();
}

/* Function to sort an array using insertion sort*/
short layerSort(void* a, void* b) {
	Object* ao = (Object*)a;
	Object* bo = (Object*)b;
	logger->inf("SORT: %s: %d | %s: %d", ao->name, ao->pos.y, bo->name, bo->pos.y);

	if (ao->pos.y == bo->pos.y) {
		logger->inf("-1");
		//*res = 0;
		return 0;
	}

	logger->inf("%d", (ao->pos.y < bo->pos.y) ? -1 : 1);
	return (ao->pos.y < bo->pos.y) ? -1 : 1;
	//return *res;
}

short addChild(Object* obj, Object* child) {
	logger->inf("==== Adding Child %s ====", child->name);
	logger->dbg("-- parent: %s\n-- child: %s", obj->name, child->name);
	
	if (obj->childs == NULL) {
		logger->dbg("-- Init child list");
		obj->childs = initListMgr();
	}

	child->pos.x += obj->pos.x;
	child->pos.y += obj->pos.y;

	Node* n = addNodeV(obj->childs, child->name, child, 0);
	if (n == NULL) {
		logger->err("==== FAIL TO ADD CHILD NODE =====");
		return 0;
	}

	logger->dbg("-- nodeID: %d", n->id);
	logger->dbg("==== Child %s Added to %s ====", child->name, obj->name);
	return 1;
}

ListManager* getObjectList() {
	static ListManager* objects = NULL;
	
	if (objects != NULL){
		return objects;
	}

	logger->inf("==== Init Object List ====");
	objects = initListMgr();
	return objects;
}


Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	Object* obj = malloc(sizeof(Object));
	
	obj->z = z;
	obj->visible = 1;
	obj->enabled = 1;

	obj->name = name;
	obj->component = comp;
	obj->container = container;
	
	obj->hit = NULL;
	obj->clip = NULL;
	obj->click = click;
	obj->hover = hover;
	
	obj->childs = NULL;
	obj->hitbox.x = obj->hitbox.y = obj->hitbox.w = obj->hitbox.h = 0;

	if (pos != NULL)
	{
		obj->pos.x = pos->x;
		obj->pos.y = pos->y;
		obj->pos.w = pos->w;
		obj->pos.h = pos->h;
	}
	else{
		obj->pos.x = 0;
		obj->pos.y = 0;
		obj->pos.w = SCREEN_W; 
		obj->pos.h = SCREEN_H;
	}


	logger->dbg(
		"--name: %s\n--z-index: %d\n--click: %d\n--hover: %d\n--hit: %d\n--pos: x: %d | y:%d | w:%d | h:%d",
		obj->name,
		obj->z,
		(obj->click != NULL),
		(obj->hover != NULL),
		(obj->hit != NULL),
		obj->pos.x,
		obj->pos.y,
		obj->pos.w,
		obj->pos.h
	);

	return obj;
}

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	logger->inf("=== Adding Object %s ===", name);
	Object* obj = genObject(name, comp, pos, z, click, hover, container);
	
	ListManager* objects = getObjectList();
	logger->dbg("-- Adding Node");
	
	Node* n = addNodeV(objects, name, obj, 1);
	if (n == NULL) {
		logger->err("==== Fail to insert object in list ====");
		return NULL;
	}

	logger->dbg("-- nodeID: %d", n->id);
	obj->id = n->id;

	logger->dbg("-- Setting Layer");
	setObjectLayer(obj, z);

	logger->dbg("==== Object %s Added ====", obj->name);

	return obj;
}

Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
	return genObject(name, comp, pos, z, NULL, NULL, NULL);
}

Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
	return addObject(name, comp, pos, z, NULL, NULL, NULL);
}

void deleteObject(Object* obj) {
	logger->inf("===== Deleting Object ====");
	logger->inf("--name: %s", obj->name);
	Node* layer = getNode(getLayers(), obj->z);

	if (obj->childs != NULL) {
		logger->dbg("-- Delete Object Childs");
		
		Node* childNode = NULL;
		while((childNode = listIterate(obj->childs, childNode)) != NULL) {
			Object* child = (Object*) childNode->value;
			
			if (child->clip != NULL) {
				free(child->clip);
			}
			
			logger->dbg("-- child: %s", child->name);
			deleteObject(child);

			logger->dbg("-- delete Node");
			deleteNode(obj->childs, childNode->id);
		}

		deleteList(obj->childs);
		logger->dbg("-- Childs Cleared");
	}

	if (layer != NULL) {
		logger->dbg("-- Deleting From : %s", layer->name);
		deleteNodeByName(layer->value, obj->name);

		logger->dbg("-- Remove From Anim");
		animRemoveObject(obj);
	}

	//if (obj->clip != NULL) {
	//	logger->dbg("-- Free Clip Rect");
	//	free(obj->clip);
	//}

	logger->dbg("-- Delete Object");
	ListManager* objects = getObjectList();
	deleteNode(objects, obj->id);
	
	logger->inf("===== DELETE OBJECT DONE ====");
}

Object* generateButton(Button* btn) {
	AssetMgr* ast = getAssets();
	logger->inf("===== Generating Button ====");
	
	logger->dbg(
		"--name: %s\n--text: %s\n--click: %d\n--hover: %d\n--image: %s\n--font: %s\n--pos: x: %d | y:%d | w:%d | h:%d",
		btn->name,
		btn->text,
		(btn->click != NULL),
		(btn->hover != NULL),
		btn->imgPath,
		btn->font,
		btn->pos.x,
		btn->pos.y,
		btn->pos.w,
		btn->pos.h
	);


	logger->dbg("-- Loading Image");
	SDL_Surface* img = ast->getImg(btn->imgPath);
	if (img == NULL){
		logger->err("-- Fail to get Image: %s", btn->imgPath);
		logger->dbg("==== BUTTON DONE ====");
		return NULL;
	}


	logger->dbg("-- Loading Font");
	TTF_Font* font = ast->getFont("pf", FONT_LG);
	if (font == NULL) {
	    logger->err("Fail to load Font: %s", btn->font);
	    logger->err("Sdl err: %s", TTF_GetError());
		logger->err("==== BUTTON DONE ====");
	    return NULL;
	}


	logger->dbg("-- Generating Text");
	SDL_Surface* txt = TTF_RenderText_Solid(font, btn->text, btn->color);
	if (txt == NULL) {
	    logger->err("Fail to Render Text");
		logger->err("==== BUTTON DONE ====");
	    return NULL;
	}

	logger->dbg("-- Adding Button");
	btn->imgObj = addObject(btn->name, img, &btn->pos, btn->z, btn->click, btn->hover, btn);

	logger->dbg("-- Adding Text");
	SDL_Rect rect = {
		(btn->pos.w / 2) - (FONT_LG * (strlen(btn->text) / 4)),
		1,
		btn->pos.w,
		btn->pos.h,
	};
	
	btn->txtObj = genSimpleObject("btnText", txt, &rect, 3);
	
	logger->dbg("-- Adding child");
	addChild(btn->imgObj, btn->txtObj);


	if (btn->hasAnim){
		logger->dbg("-- Animating Button");
		logger->dbg("Setting Btn ANIM: x=%d | y=%d, | t=%f | d=%f", btn->anim->pos.x, btn->anim->pos.y, btn->anim->time, btn->anim->delay);
		moveTo(btn->imgObj, btn->anim->pos.x, btn->anim->pos.y, btn->anim->time, btn->anim->delay);
	}

	logger->dbg("==== BUTTON DONE ====");
	return btn->imgObj;
}
