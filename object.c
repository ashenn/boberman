#include "object.h"

ListManager* getObjectList() {
	static ListManager* objects = NULL;
	
	if (objects != NULL){
		return objects;
	}

	objects = initListMgr();
	return objects;
}


Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	Object* obj = malloc(sizeof(Object));
	
	obj->visible = 1;
	obj->enabled = 1;
	obj->name = name;
	obj->click = click;
	obj->hover = hover;
	obj->component = comp;
	obj->container = container;
	obj->childObj = NULL;

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
		obj->pos.w = 0;
		obj->pos.h = 0;
	}

	return obj;
}

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	//logger->inf("Adding Component: '%s'", name);
	Object* obj = genObject(name, comp, pos, z, click, hover, container);

	
	ListManager* objects = getObjectList();
	Node* n = addNodeV(objects, name, obj, 0);
	if (n == NULL) {
		//logger->err("Fail to insert object in list");
	}

	obj->id = n->id;

	setObjectLayer(obj, z);
	Object* test = (Object*) n->value;

	//logger->inf("Current Objects:");

	return obj;
}

Object* genSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
	return genObject(name, comp, pos, z, NULL, NULL, NULL);
}

Object* addSimpleObject(char* name, void* comp, SDL_Rect* pos, short z) {
	return addObject(name, comp, pos, z, NULL, NULL, NULL);
}


void deleteObject(Object* obj) {
	logger->inf("Deleting: %s", obj->name);
	Node* layer = getNode(getLayers(), obj->z);

	if (layer != NULL) {
		logger->dbg("Deleting From : %s", layer->name);
		deleteNodeByName(layer->value, obj->name);
	}

	logger->dbg("TEST: %s", obj->name);
	if (obj->component != NULL) {
		logger->dbg("Free Surface");
		SDL_FreeSurface(obj->component);
	}

	if (obj->childObj != NULL) {
		logger->dbg("Delete Object Child");
		SDL_FreeSurface(obj->childObj->component);
		free(obj->childObj);
	}

	logger->dbg("Delete Remove From Anim");
	animRemoveObject(obj);

	logger->dbg("Delete Object");
	ListManager* objects = getObjectList();
	deleteNode(objects, obj->id);
	logger->dbg("Delete Object DONE");
}

Object* generateButton(Button* btn) {
	logger->inf("Generating Button: %s => %d | %d", btn->name, btn->pos.x, btn->pos.y);
	
	char imgPath[25];
	sprintf(imgPath, "asset/%s.png", btn->imgPath);
	//logger->dbg("IMG Path: %s", imgPath);
	SDL_Surface* img = IMG_Load(imgPath);

	if (img == NULL){
		//logger->err("Fail to get Image: %s", imgPath);
		return NULL;
	}

	//logger->dbg("BTN POS: %d | %d | %d | %d", btn->pos.x, btn->pos.y, btn->pos.w, btn->pos.h);
	btn->imgObj = addObject(btn->name, img, &btn->pos, btn->z, btn->click, btn->hover, btn);

	SDL_Color text_color = {255,255,255};
	char fontpath[] = "font/pf.ttf";
	TTF_Font* font = TTF_OpenFont(fontpath, 24);

	if (font == NULL) {
	    //logger->err("Fail to load Font: %s", fontpath);
	    //logger->err("Sdl err: %s", TTF_GetError());
	    return NULL;
	}

	SDL_Surface* txt = TTF_RenderText_Solid(font, btn->text, text_color);

	if (txt == NULL) {
	    //logger->err("Fail to Render Text");
	    return NULL;
	}

	SDL_Rect rect = {
		btn->pos.x + (btn->pos.w / 2) - (24 * (strlen(btn->text) / 4)),
		btn->pos.y + (24 / 5),
		btn->pos.w,
		btn->pos.h,
	};

	btn->imgObj->childObj = btn->txtObj = genSimpleObject("btnText", txt, &rect, 3);

	if (btn->hasAnim){
		logger->err("Setting Btn ANIM: x=%d | y=%d, | t=%f | d=%f", btn->x, btn->y, btn->time, btn->delay);
		moveTo(btn->imgObj, btn->x, btn->y, btn->time, btn->delay);
	}

	return btn->imgObj;
}
