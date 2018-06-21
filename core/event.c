#include "event.h"

void EventQuit() {
	changeGameStatus(GAME_QUIT);
}

void EventKeyUp(int key, int curKey) {
	Player* p = getPlayer();

	if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) {
		if (p && p->alive && key == curKey) {
			sendCommand("stopmove", 0);
			//p->clipIndex = 0;
			//animRemoveObject(p->object);
			//updatePlayerClip(p);
		}
	}
}

int EventKeyDown(int key, int curKey) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_EVNT;
	logger->err("=== Key Event ===");

	if (key == SDLK_ESCAPE) {
		logger->dbg("-- Event: Quit");
		logger->err("############# Event: Quit ##################");
		EventQuit();
		return curKey;
	}

	if(game->status != GAME_LOBY && game->status != GAME_START && game->status != GAME_RUNNING) {
		return curKey;
	}

	Player* p = getPlayer();
	if (p == NULL) {
		logger->err("-- No Player For Event");
		return curKey;
	}

	if (!p->alive) {
		logger->err("-- Player is Dead");
		return curKey;
	}

	switch (key) {
		case SDLK_UP:
			logger->dbg("-- Player Move UP");
			curKey = SDLK_UP;

			if (p && p->alive) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				//playerMove(p, UP);

				sendCommand("move", UP);
			}
			break;

		case SDLK_DOWN:
			logger->dbg("-- Player Move DOWN");
			curKey = SDLK_DOWN;

			if (p && p->alive) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				//playerMove(p, DOWN);

				sendCommand("move", DOWN);
			}
			break;

		case SDLK_LEFT:
			logger->dbg("-- Player Move LEFT");
			curKey = SDLK_LEFT;

			if (p && p->alive) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				//playerMove(p, LEFT);

				sendCommand("move", LEFT);
			}
			break;

		case SDLK_RIGHT:
			logger->dbg("-- Player Move RIGHT");
			curKey = SDLK_RIGHT;

			if (p && p->alive) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				//playerMove(p, RIGHT);

				sendCommand("move", RIGHT);
			}
			break;

		case SDLK_SPACE:
			logger->dbg("-- Player Place Bomb");
			if (p && p->alive) {
				//placeBomb(p);

				sendCommand("bomb", 0);
			}
			break;
	}

	return curKey;
}

void EventMouseClick(SDL_Event* event) {
	Object* clicked = NULL;
	
	Game* game = getGame();
	logger->enabled = game->flags & DBG_EVNT;

	logger->dbg("-- EVT KEY CLICK");

	clicked = getClicked(event->button.x, event->button.y, 0);
	if (clicked != NULL && clicked->enabled && clicked->click != NULL) {
		logger->dbg("-- Clicked: %s", clicked->name);
		clicked->click(clicked);
	}
}

void EventMouseMove(SDL_Event* event) {
	static int lastX = 0;
	static int lastY = 0;
	
	Game* game = getGame();
	logger->enabled = game->flags & DBG_EVNT;

	Object* hovered = NULL;
	Object** curHovered = getHovered();

	logger->dbg("-- EVT KEY MOVE");

	if (lastX == event->button.x && lastY == event->button.y) {
		return;
	}

	lastX = event->button.x;
	lastY = event->button.y;

	hovered = getClicked(event->button.x, event->button.y, 1);
	logger->dbg("Getting Cur Hover");
	curHovered = getHovered();

	if (hovered == *curHovered) {
		return;
	}

	if (*curHovered != NULL && hovered != *curHovered){
		buttonUnHover(*curHovered);
	}
	
	*curHovered = hovered;

	if (hovered != NULL && hovered->enabled && hovered->hover != NULL) {

		logger->inf("-- Calling hover For: %s", hovered->name);
		hovered->hover(hovered);
	}
}

void handleEvents() {
	SDL_Event event;
	Game* game = getGame();
	logger->enabled = game->flags & DBG_EVNT;

	static int lastX = 0;
	static int lastY = 0;
	static int curKey = -1;
	
	Object* clicked = NULL;
	Object* hovered = NULL;
	Object** curHovered = getHovered();

	int key;
	Player* p = getPlayer();
	logger->inf("==== EVENTS ====");
	while (SDL_PollEvent(&event)) {
		key = -1;

		switch (event.type) {
			case SDL_QUIT:
				EventQuit();
				break;

			case SDL_KEYUP:
				key = event.key.keysym.sym;
				EventKeyUp(key, curKey);
				break;

			case SDL_KEYDOWN:
				key = event.key.keysym.sym;
				logger->dbg("-- EVT KEY DOWN: %s", SDL_GetKeyName(key));
				logger->dbg("-- EVT KEY VALUE: %d", key);

				curKey = EventKeyDown(key, curKey);
				break;

			case SDL_MOUSEBUTTONDOWN:
				EventMouseClick(&event);
            	break;

			case SDL_MOUSEMOTION:
				EventMouseMove(&event);
            	break;
		}
	}

	logger->dbg("==== EVENTS DONE ====");
}