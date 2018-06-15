#include "game.h"

void* sayHello() {
	logger->err("Hello !");
}

void* sayGoodbye() {
	logger->err("Goodbye !");

	clearObjects();

	AssetMgr* ast = getAssets();
	SDL_Surface* bg = ast->getImg("title");
	addSimpleObject("Background", bg, NULL, 1);

	int i;
	Button** btns = getMenu();
	for (i = 0; btns[i] != NULL; ++i){
		generateButton(btns[i]);
	}
}

void tick() {
	Game* game = getGame();

	if (game->status != GAME_LOBY) {
		return;
	}

	resetPlayersBomb();
	clearOutdatedObjects();
}

void tickWait(int next) {
	int now = SDL_GetTicks();
	//logger->dbg("NEXT: %d", next);
	//logger->dbg("NOW: %d", now);

	//logger->dbg("DELAY: %d\n", next - now);
    if(next <= now){
        return;
    }

	SDL_Delay(next - now);
}

void* loadMap() {
	changeGameStatus(GAME_LOBY);
}

void launchSate(short status) {
	int y = 0;
	int i = 0;
	int now = 0;
	int add = 0;
	int nextFrame = 0;
	Game* game = getGame();

	//logger->err("GAME: Un-Lock");
	unlock(DBG_VIEW);

	while(game->status == status) {
		//logger->war("GAME: Ask-Lock");
		lock(DBG_VIEW);
		//logger->war("GAME: Lock");

		logger->enabled = game->flags & DBG_STATE;

		now = SDL_GetTicks();
		add = (int)(1000 / (float)FPS);
	    
	    nextFrame = now + add;

		handleEvents();
    	
    	if (game->status == GAME_LOBY) {    		
    		handleHits();
    	}

		logger->enabled = game->flags & DBG_STATE;
		logger->inf("==== Tick END ====");


		//logger->err("GAME: Un-Lock");
		unlock(DBG_VIEW);

		SDL_Delay(5);
	}

	logger->dbg("===== STATE END =====");
}

void renderMap() {
	//logger->err("MAP: Ask-Lock");
	lock(DBG_STATE);
	//logger->err("MAP: Lock");

	Game* game = getGame();

	logger->enabled = game->flags & DBG_MAP;

	logger->inf("==== LOADING MAP ====");
	clearObjects();

	AssetMgr* ast = getAssets();
	SDL_Surface* bg = ast->getImg("map");
	addSimpleObject("Background", bg, NULL, 1);

	generateWalls();

	Player* p = genPlayer("PLAYER-1");
	initPlayer(p);

	launchSate(GAME_LOBY);
}

Game* getGame() {
	static Game* game = NULL;
	
	if (game != NULL) {
		return game;
	}

	logger->enabled = 1;
	logger->inf("==== INIT Game* game ====");	
	game = malloc(sizeof(Game));

	game->flags = NO_FLAG;
	game->status = GAME_MENU;
	game->renderThread = NULL;

	game->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	game->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	return game;
}

void quitGame() {
	logger->enabled = 1;
	logger->inf("==== Quitting Game* game ====");	
	Game* game = getGame();
	game->status = GAME_QUIT;
}


Button** getMenu() {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_MENU;

	logger->inf("==== Getting Menu ====");	
	

	Button* btnJoin = malloc(sizeof(Button));
		btnJoin->z = 2;
		btnJoin->name = "Find";
		btnJoin->text = "Find";
		btnJoin->font = "pf";
		btnJoin->color.r = 255;
		btnJoin->color.g = 255;
		btnJoin->color.b = 255;

		btnJoin->fontSize = FONT_LG;
		btnJoin->imgPath = "lg-button-green";
		btnJoin->imgHoverPath = NULL;
		btnJoin->imgObj = NULL;
		btnJoin->txtObj = NULL;
		btnJoin->click = (void*) loadMap;
		btnJoin->hover = (void*) buttonHover;
		btnJoin->hasAnim = 1;

		// Define Button Size
		btnJoin->pos.w = LG_BTN_W;
		btnJoin->pos.h = LG_BTN_H;

		// Define Position
		btnJoin->pos.x = SCREEN_W + 5;
		btnJoin->pos.y = percent(45, SCREEN_H);
		
		// Define Animation Position
		btnJoin->anim = malloc(sizeof(AnimParam));
		btnJoin->anim->time = 0.7f;
		btnJoin->anim->delay = 0;
		btnJoin->anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
		btnJoin->anim->pos.y = percent(45, SCREEN_H);
		
		btnJoin->anim->pos.w = 0;
		btnJoin->anim->pos.h = 0;
	
		logger->dbg(
			"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
			btnJoin->name,
			btnJoin->pos.x,
			btnJoin->pos.y,
			btnJoin->pos.w,
			btnJoin->pos.h,
			btnJoin->anim->pos.x,
			btnJoin->anim->pos.y
		);


	Button* btnHost = malloc(sizeof(Button));
		btnHost->z = 2;
		btnHost->name = "Host";
		btnHost->text = "Host";
		btnHost->font = "pf";
		btnHost->color.r = 255;
		btnHost->color.g = 255;
		btnHost->color.b = 255;

		btnHost->fontSize = FONT_LG;
		btnHost->imgPath = "lg-button-green";
		btnHost->imgHoverPath = NULL;
		btnHost->imgObj = NULL;
		btnHost->txtObj = NULL;
		btnHost->click = (void*) sayGoodbye;
		btnHost->hover = (void*) buttonHover;
		btnHost->hasAnim = 1;

		// Define Button Size
		btnHost->pos.w = LG_BTN_W;
		btnHost->pos.h = LG_BTN_H;

		// Define Position
		btnHost->pos.x = SCREEN_W + 5;
		//btnHost->pos.y = 230;
		btnHost->pos.y = btnJoin->anim->pos.y + (LG_BTN_H  * 1.25);


		// Define Animation Position
		btnHost->anim = malloc(sizeof(AnimParam));
		btnHost->anim->time = 0.7f;
		btnHost->anim->delay = 0.2f;
		btnHost->anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
		//btnHost->anim->pos.y = 230;
		btnHost->anim->pos.y = btnJoin->anim->pos.y + (LG_BTN_H  * 1.25);
		
		btnHost->anim->pos.w = 0;
		btnHost->anim->pos.h = 0;

		logger->dbg(
			"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
			btnHost->name,
			btnHost->pos.x,
			btnHost->pos.y,
			btnHost->pos.w,
			btnHost->pos.h,
			btnHost->anim->pos.x,
			btnHost->anim->pos.y
		);


	Button* btnQuit = malloc(sizeof(Button));
		btnQuit->z = 2;
		btnQuit->name = "Quit";
		btnQuit->text = "Quit";
		btnQuit->font = "pf";
		btnQuit->color.r = 255;
		btnQuit->color.g = 255;
		btnQuit->color.b = 255;

		btnQuit->fontSize = FONT_LG;
		btnQuit->imgPath = "lg-button-red";
		btnQuit->imgHoverPath = NULL;
		btnQuit->imgObj = NULL;
		btnQuit->txtObj = NULL;
		btnQuit->click = (void*) quitGame;
		btnQuit->hover = (void*) buttonHover;
		btnQuit->hasAnim = 1;

		// Define Button Size
		btnQuit->pos.w = LG_BTN_W;
		btnQuit->pos.h = LG_BTN_H;

		// Define Position
		btnQuit->pos.x = SCREEN_W + 5;
		//btnQuit->pos.y = 290;
		btnQuit->pos.y = btnHost->anim->pos.y + (LG_BTN_H  * 1.25);



		// Define Animation Position
		btnQuit->anim = malloc(sizeof(AnimParam));
		btnQuit->anim->time = 0.7f;
		btnQuit->anim->delay = 0.4f;
		btnQuit->anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
		//btnQuit->anim->pos.y = 290;
		btnQuit->anim->pos.y = btnHost->anim->pos.y + (LG_BTN_H  * 1.25);

		btnQuit->anim->pos.w = 0;
		btnQuit->anim->pos.h = 0;

		logger->dbg(
			"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
			btnQuit->name,
			btnQuit->pos.x,
			btnQuit->pos.y,
			btnQuit->pos.w,
			btnQuit->pos.h,
			btnQuit->anim->pos.x,
			btnQuit->anim->pos.y
		);

	Button** btns = malloc(4 * sizeof(Button*));
	btns[0] = btnJoin;
	btns[1] = btnHost;
	btns[2] = btnQuit;
	btns[3] = NULL;

	return btns;
}

void mainMenu() {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_MENU;

	clearObjects();
	AssetMgr* ast = getAssets();

	SDL_Surface* bg = ast->getImg("title2");
	addSimpleObject("Background", bg, NULL, 1);

	int i;
	Button** btns = getMenu();
	for (i = 0; btns[i] != NULL; ++i){
		generateButton(btns[i]);
	}

	launchSate(GAME_MENU);
	free(btns);
}

void changeGameStatus(short status) {
	Game* game = getGame();
	logger->enabled = game->flags & DBG_STATE;

	logger->inf("==== Changing Status: %d ====", status);
	game->status = status;
}

void* addDebugFlag(char* flag) {
	Game* game = getGame();

	if (!strcmp("hit", flag)) {
		game->flags = game->flags | DBG_HIT;
	}
	else if(!strcmp("evnt", flag)) {
		game->flags = game->flags | DBG_EVNT;
	}
	else if(!strcmp("move", flag)) {
		game->flags = game->flags | DBG_MOVE;
	}
	else if(!strcmp("view", flag)) {
		game->flags = game->flags | DBG_VIEW;
	}
	else if(!strcmp("bomb", flag)) {
		game->flags = game->flags | DBG_BOMB;
	}
	else if(!strcmp("mouse", flag)) {
		game->flags = game->flags | DBG_MOUSE;
	}
	else if(!strcmp("asset", flag)) {
		game->flags = game->flags | DBG_ASSET;
	}
 	else if(!strcmp("anim", flag)) {
		game->flags = game->flags | DBG_ANIM;
	}
	else if(!strcmp("map", flag)) {
		game->flags = game->flags | DBG_MAP;
	}
	else if(!strcmp("obj", flag)) {
		game->flags = game->flags | DBG_OBJ;
	}
	else if(!strcmp("player", flag)) {
		game->flags = game->flags | DBG_PLAYER;
	}
	else if(!strcmp("state", flag)) {
		game->flags = game->flags | DBG_STATE;
	}
	else if(!strcmp("menu", flag)) {
		game->flags = game->flags | DBG_MENU;
	}
}

void parseGameArgs(int argc, char* argv[]){
	static Arg arg1 = {
		.name = "-dbg", 
		.function = addDebugFlag, 
		.hasParam = 1, 
		.defParam = NULL, 
		.asInt = 0,
		.type="alpha"
	};

	static  Arg* args[] = {
		&arg1,
		NULL
	};

	ListManager* lst = defineArgs(args);
	parseArgs(lst, argc, argv);
	deleteList(lst);
}
