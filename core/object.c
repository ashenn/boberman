#include "object.h"

void clearOutdatedObjects() {
	Node* n = NULL;
	ListManager* objects = getObjectList();

	while((n = listIterate(objects, n)) != NULL) {
	    Object* o = (Object*) n->value;
	    if (o->lifetime == -1){
	    	continue;
	    }

	    logger->dbg("objectLife: %d", o->lifetime);
	    if (--o->lifetime <= 0){
	    	logger->dbg("Deleting: %s", o->name);
	    	n = n->prev;
	    	deleteObject(o);
	    }
	}
}

void getContainerType(ObjContType type, char* name) {
	switch (type) {
		case BUTTON:
			snprintf(name, 15, "BUTTON");
			break;
		case PLAYER:
			snprintf(name, 15, "PLAYER");
			break;
		case BOMB:
			snprintf(name, 15, "BOMB");
			break;
		
		default:
			snprintf(name, 15, "BAD CONT_INDEX");
			break;
	}
}

void deleteContainer(void* container, ObjContType type) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	logger->inf("=== DELETING CONTAINER ===");
	
	char contType[15];
	getContainerType(type, contType);
	logger->dbg("-- Type % s", contType);

	Bomb* bmb;
	Player* p;
	Button* btn;
	switch (type) {
		case BUTTON:
			btn = (Button*) container;
			free(btn->anim);
			break;

		case PLAYER:
			deletePlayer(container);
			break;

		default:
			break;
	}

	logger->dbg("-- Free");
	free(container);
	logger->dbg("=== DELETING CONTAINER DONE ===");
}

void clearObjects() {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	Node* n = NULL;
	logger->dbg("==== CLEARING Objects ====");
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
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	Object* ao = (Object*)a;
	Object* bo = (Object*)b;
	//logger->err("SORT: %s: %d | %s: %d", ao->name, ao->pos.y, bo->name, bo->pos.y);

	if (ao->pos.y == bo->pos.y) {
		//logger->err("0");
		return 0;
	}

	//logger->err("%d", (ao->pos.y < bo->pos.y) ? -1 : 1);
	return (ao->pos.y < bo->pos.y) ? -1 : 1;
}

short addChild(Object* obj, Object* child) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	logger->inf("==== Adding Child %s ====", child->name);
	logger->dbg("-- parent: %s\n-- child: %s", obj->name, child->name);
	
	if (obj->childs == NULL) {
		logger->dbg("-- Init child list");
		obj->childs = initListMgr();
	}
	

	logger->dbg("-- Child Pos: x: %d + %d, y: %d + %d", child->pos.x, obj->pos.x, child->pos.y, obj->pos.y);
	child->pos.x += obj->pos.x;
	child->pos.y += obj->pos.y;
	logger->dbg("-- Child Pos: x: %d, y: %d", child->pos.x, child->pos.y);

	Node* n = addNodeV(obj->childs, child->name, child, 1);
	if (n == NULL) {
		logger->err("==== FAIL TO ADD CHILD NODE =====");
		return 0;
	}

	if (obj->parent == NULL) {
		child->parent = obj;
	}
	else{
		child->parent = obj->parent;
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

	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	logger->inf("==== Init Object List ====");
	objects = initListMgr();
	return objects;
}


Object* genObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	Object* obj = malloc(sizeof(Object));
	
	obj->z = z;
	obj->visible = 1;
	obj->enabled = 1;
	obj->lifetime = -1;

	obj->name = malloc(strlen(name)+1);
	strcpy(obj->name, name);

	obj->component = comp;
	obj->container = container;
	
	obj->clip = NULL;
	obj->click = click;
	obj->hover = hover;
	
	obj->color = 0;
	obj->parent = NULL;
	obj->childs = NULL;
	obj->onDelete = NULL;
	obj->collision = NULL;
	

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

	//obj->threadCond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	//obj->threadMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	//pthread_cond_signal(&obj->threadCond);
	//pthread_mutex_unlock(&obj->threadMutex);

	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	logger->dbg(
		"--name: %s\n--z-index: %d\n--click: %d\n--hover: %d\n--pos: x: %d | y:%d | w:%d | h:%d",
		obj->name,
		obj->z,
		(obj->click != NULL),
		(obj->hover != NULL),
		obj->pos.x,
		obj->pos.y,
		obj->pos.w,
		obj->pos.h
	);

	return obj;
}

Object* addObject(char* name, void* comp, SDL_Rect* pos, short z, void* click, void* hover, void* container) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

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
	logger->dbg("-- Added To Layer");

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
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

	logger->inf("===== Deleting Object ====");
	logger->dbg("--name: %s", obj->name);
	Node* layer = getNode(getLayers(), obj->z);

	if (obj->onDelete != NULL) {
		logger->dbg("--Calling On Delete Func");
		obj->onDelete(obj);
	}

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
			childNode = NULL;
		}

		deleteList(obj->childs);
		logger->dbg("-- Childs Cleared");
	}

	if (layer != NULL) {
		logger->dbg("-- Deleting From : %s", layer->name);
		deleteNode(layer->value, obj->id);

		logger->dbg("-- Remove From Anim");
		animRemoveObject(obj);
	}

	if (obj->container != NULL) {
		deleteContainer(obj->container, obj->containerType);
		obj->container = NULL;
	}

	if (obj->collision != NULL) {
		ListManager* hitObjects = getHitObjectList();
		logger->dbg("-- Removing collision: %s", obj->name);
		deleteNode(hitObjects, obj->collision->id);

		free(obj->collision);
	}

	logger->dbg("-- Delete Object");
	free(obj->name);
	obj->name = NULL;

	ListManager* objects = getObjectList();
	deleteNode(objects, obj->id);
	
	logger->dbg("===== DELETE OBJECT DONE ====");
}

Object* generateButton(Button* btn) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_OBJ;

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
	
	btn->imgObj->container = btn;
	btn->imgObj->containerType = BUTTON;

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