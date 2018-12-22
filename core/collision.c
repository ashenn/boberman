#include "collision.h"

ListManager* getHitObjectList() {
	static ListManager* objects = NULL;
	
	if (objects != NULL){
		return objects;
	}

	enableLogger(DBG_HIT);

	logger->inf("==== Init Object Hit List ====");
	objects = initListMgr();
	return objects;
}

Object* getObjectByCollision(Collision* col2, short blocking, Object* ignore) {
	Node* n = NULL;
	Object* o = NULL;
	Game* game = getGame();
	ListManager* objects = getHitObjectList();

	enableLogger(DBG_HIT);
	logger->inf("==== SEARCH COLLISION ====");
	while((n = listIterate(objects, n)) != NULL) {
		if (n == NULL || n->value == NULL)
		{
			continue;
		}

		o = (Object*) n->value;
		if (o == NULL)
		{
			continue;
		}

		if (o == ignore) {
			continue;
		}
		logger->dbg("-- Obj: %s", o->name);

		Collision* col = o->collision;
		if (col == NULL) {
			continue;
			
		} else if(!col->enabled) {
			continue;
		} 
		else if((blocking != -1 && blocking != col->blocking)) {
			continue;
		}

		Collision tmpCol;
		prepareCol(o, &tmpCol);

		short collides = doesCollide(&tmpCol, col2);

		if (collides) {
			logger->dbg("--- Collides: %s", o->name);
			return o;
		}
		
		logger->dbg("-- No Col");
	}

	logger->dbg("-- No Collision Found");
	return NULL;
}

short doesObjectCollides(Object* o, Object* o2) {
	if (o2->collision == NULL || !o2->collision->enabled) {
		logger->dbg("-- Skipping");
    	return 0;
    }

    if (o->collision->flag == COL_NONE || o2->collision->flag == COL_NONE || o2->collision->flag == COL_WALL) {
		logger->dbg("-- Skipping COL NONE");
		return 0;
    }


    if (o->collision->colFlags != COL_ALL && o2->collision->colFlags != COL_ALL) {
	    if (!(o->collision->colFlags & o2->collision->flag) && !(o2->collision->colFlags & o->collision->flag)) {
			logger->dbg("-- Skipping Flags Incompatible");
			return 0;
	    }
    }

	Collision col;
	prepareCol(o, &col);

	Collision col2;
	prepareCol(o2, &col2);

	return doesCollide(&col, &col2);
}

short doesCollide(Collision* col, Collision* col2) {
	short width1 =	
		(col2->pos.x + col2->pos.w >= col->pos.x) 
		&&
		(col2->pos.x + col2->pos.w <= (col->pos.x + col->pos.w))
    ;

	short width2 =
		(col->pos.x + col->pos.w >= col2->pos.x)
		&&
		(col->pos.x + col->pos.w <= col2->pos.x + col2->pos.w)
    ;

    short height1 =	
    	(col2->pos.y + col2->pos.h >= col->pos.y) &&
		(col2->pos.y + col2->pos.h <= col->pos.y + col->pos.h)
    ;
    
    short height2 =	
    	(col->pos.y + col->pos.h >= col2->pos.y) &&
		(col->pos.y + col->pos.h <= col2->pos.y + col2->pos.h)
    ;

    return (width1 || width2) && (height1 || height2);
}

void handleHits() {
	enableLogger(DBG_HIT);

	logger->inf("==== Verifying Hits ====");
	Node* n = NULL;
	Node* n2 = NULL;
	ListManager* objects = getHitObjectList();

	Object* o = NULL;
	Object* o2 = NULL;
	short deleted = 0;
	Game* game = getGame();

	// logger->err("==== Verifying Hits ====");
	while((n = listIterate(objects, n)) != NULL) {
	    // logger->err("### LOOP TOP ###");
		deleted = 0;
		if (n == NULL || n->value == NULL)
		{
			continue;
		}

	    o = (Object*) n->value;

	    if (o == NULL) {
	    	continue;
	    }

		Collision* col = o->collision;
	    if (col == NULL){
		    logger->err("+++ Node: %s", n->name);
		    logger->err("+++ Object: %s", o->name);
		    logger->err("Collision Is NULL");
	    	continue;
	    }
	    else if(!col->enabled) {
	    	continue;
	    	
	    }
	    else if(col->fnc == NULL) {
	    	continue;
	    }


		logger->dbg("++++ Object: %s", o->name);

		short collides = 0;
		while((n2 = listIterate(objects, n2)) != NULL) {
		    o2 = (Object*) n2->value;
			logger->dbg("-- Checking: %s", o2->name);
			
			Collision* col2 = o2->collision;

			if (o == o2) {
				logger->dbg("-- Skipping");
		    	continue;
			}

			if (o->parent != NULL) {
				if (o->parent == o2 || o->parent == o2->parent) {
					logger->dbg("-- Skipping");
			    	continue;
				}
			}

			if (o2->parent != NULL) {
				if (o2->parent == o || o2->parent == o->parent) {
					logger->dbg("-- Skipping");
			    	continue;
				}
			}

			/*
				if (col2 == NULL || !col2->enabled) {
					logger->dbg("-- Skipping");
			    	continue;
			    }

			    if (col->flag == COL_NONE || col2->flag == COL_NONE || col2->flag == COL_WALL) {
					logger->dbg("-- Skipping COL NONE");
					continue;
			    }


			    if (col->colFlags != COL_ALL && col2->colFlags != COL_ALL) {
				    if (!(col->colFlags & col2->flag) && !(col2->colFlags & col->flag)) {
						logger->err("-- Skipping Flags Incompatible");
						continue;
				    }
			    }
			*/

			short objCollide = doesObjectCollides(o, o2);
		    collides = collides || objCollide;

		    if (objCollide){
				logger->dbg("Collision %s | %s", o->name, o2->name);
				logger->dbg("Collision Function: %p", col->fnc);
		    	col->fnc(o, o2);
		    }

		    if (objCollide && col->deleteOnCol) {
				logger->dbg("DELETING OBJECT  %s", o->name);
		    	n = n->prev;
		    	deleteObject(o);
		    	deleted = 1;
		    	break;
		    }
		}

		if (!deleted) {
			if (collides) {
				if (game->flags & DBG_HIT) {
					col->dbgObj->clip->x = 320;
				}
			}
			else if (game->flags & DBG_HIT) {
	    		col->dbgObj->clip->x = 0;
			}
		}

		n2 = NULL;
		o2 = NULL;
		logger->dbg("+++++++++++++++");
	}
	
	logger->dbg("==== Verifying Hits DONE ====");
}

void setHitBox(Object* obj, SDL_Rect rect, short blocking, int flag) {
	enableLogger(DBG_HIT);

	logger->inf("===== Setting Hiy Box ====");
	logger->dbg("-- Object: %s", obj->name);
	logger->dbg("-- x: %d, y: %d, w: %d, h: %d", rect.x, rect.y, rect.w, rect.h);

	Collision* col = obj->collision;
	if (col == NULL) {
		col = malloc(sizeof(Collision));
		col->fnc = NULL;
		col->enabled = 1;
		col->flag = flag;
		col->dbgObj = NULL;
		col->deleteOnCol = 0;
		col->colFlags = COL_NONE;
		
		obj->collision = col;
	}

	col->pos.x = rect.x;
	col->pos.y = rect.y;
	col->pos.w = rect.w;
	col->pos.h = rect.h;
	col->blocking = blocking;

	char boxName[35];
	snprintf(boxName, 35, "hitBox-%s", obj->name);
	
	ListManager* objects = getHitObjectList();
	Node* hitNode = addNodeV(objects, boxName, obj, 0);
	col->id = hitNode->id;

	Game* game = getGame();
	if (!(game->flags & DBG_HIT)) {
		return;
	}

	Node* n = NULL;	
	Object* hitObj = NULL;
	if (obj->childs != NULL) {
		n = getNodeByName(obj->childs, boxName);
	}

	if (n != NULL){
		hitObj = (Object*) n->value;
		hitObj->pos.x = rect.x + obj->pos.x;
		hitObj->pos.y = rect.y + obj->pos.y;
		hitObj->pos.w = rect.w;
		hitObj->pos.h = rect.h;

	}
	else if(hitObj == NULL){
		AssetMgr* ast = getAssets();
		logger->dbg("-- Creating Hit Object: %s", boxName);
		hitObj = genSimpleObject(boxName, ast->getImg("hitbox"), &rect, 3);
		hitObj->clip = malloc(sizeof(SDL_Rect));
		
		hitObj->clip->x = 0;
		hitObj->clip->y = 0;
		hitObj->clip->w = rect.w;
		hitObj->clip->h = rect.h;

		col->dbgObj = hitObj;
		addChild(obj, hitObj);

	}
	
	logger->enabled = 0;
}

void prepareCol(Object* obj, Collision* col) {
	col->pos.h = obj->collision->pos.h;
	col->pos.w = obj->collision->pos.w;
	col->pos.x = obj->collision->pos.x + obj->pos.x;
	col->pos.y = obj->collision->pos.y + obj->pos.y;
}

void prepareColAtPos(Object* obj, Collision* col, int x, int y) {
	col->pos.h = obj->collision->pos.h;
	col->pos.w = obj->collision->pos.w;
	col->pos.x = obj->collision->pos.x + x;
	col->pos.y = obj->collision->pos.y + y;
}

short canMoveTo(Object* obj, int x, int y) {
	Game* game = getGame();
	if (game->status < GAME_LOBY){
		return true;
	}

	enableLogger(DBG_HIT);	
	logger->inf("==== Can Move To ====");
	logger->dbg("--obj: %s\n-- from: x: %d, y: %d\n-- to: x: %d, y: %d", obj->name, obj->pos.x, obj->pos.y, x, y);
	
	if (obj->collision == NULL || !obj->collision->enabled){
		logger->dbg("-- No Col To Object Return True");
		return 1;
	}

	Collision col;
	prepareColAtPos(obj, &col, x, y);

	logger->dbg("--Col Pos: x: %d, y: %d, w: %d, h: %d", col.pos.x, col.pos.y, col.pos.w, col.pos.h);
	Object* colObj = getObjectByCollision(&col, 1, obj);
	
	if (colObj != NULL && (game->status & DBG_HIT)) {
		logger->dbg("-- Object Collides With: %s", colObj->name);
	}

	return (colObj == NULL);
}