#include "event.h"

void EventQuit() {
	logger->dbg("-- EVT QUIT");
	changeGameStatus(GAME_QUIT);
}

void EventKeyUp(int key, int curKey) {
	Player* p = getPlayer();

	if (key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT) {
		if (p && key == curKey) {
			p->clipIndex = 0;
			animRemoveObject(p->object);
			updatePlayerClip(p);
		}
	}
}

int EventKeyDown(int key, int curKey) {
	Player* p = getPlayer();

	switch (key) {
		case SDLK_ESCAPE:
		case SDLK_q:
			EventQuit();
			break;

		case SDLK_UP:
			curKey = SDLK_UP;

			if (p) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				playerMove(p, UP);
			}
			break;

		case SDLK_DOWN:
			curKey = SDLK_DOWN;

			if (p) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				playerMove(p, DOWN);
			}
			break;

		case SDLK_LEFT:
			curKey = SDLK_LEFT;

			if (p) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				playerMove(p, LEFT);
			}
			break;

		case SDLK_RIGHT:
			curKey = SDLK_RIGHT;

			if (p) {
				animRemoveObject(p->object);
				p->clipIndex = 0;
				playerMove(p, RIGHT);
			}
			break;

		case SDLK_SPACE:
			if (p) {
				logger->err("-- Placing Bomb");
				placeBomb(p);
			}
			break;
		
		default:
			logger->err("-- UN HANDEL %d", key);
			logger->err("-- TEST %d", SDLK_SPACE);
			break;
	}

	return curKey;
}

void EventMouseClick(SDL_Event* event) {
	Object* clicked = NULL;
	
	logger->dbg("-- EVT KEY BUTTON");

	clicked = getClicked(event->button.x, event->button.y, 0);
	if (clicked != NULL && clicked->enabled && clicked->click != NULL) {
		clicked->click(clicked);
	}
}

void EventMouseMove(SDL_Event* event) {
	static int lastX = 0;
	static int lastY = 0;
	
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
	Game* game = NULL;
	
	static int lastX = 0;
	static int lastY = 0;
	static int curKey = -1;
	
	Object* clicked = NULL;
	Object* hovered = NULL;
	Object** curHovered = getHovered();

	if (game == NULL) {
		game = getGame();
	}

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
				logger->err("-- EVT KEY DOWN: %s", SDL_GetKeyName(key));
				logger->err("-- EVT KEY VALUE: %d", key);

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

	logger->dbg("==== DONE ====");
}