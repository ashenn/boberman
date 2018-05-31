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


void tickWait(int now) {
	int delay = 0;
	int t_deb = 0;
	int t_fin = 0;
	int t_boucle = 1000 / FPS;

	t_fin = SDL_GetTicks();
    if (t_fin - t_deb < t_boucle) {
	
		t_fin = SDL_GetTicks();

		delay = t_boucle - (t_fin - t_deb);
		if (delay > 0){
    		SDL_Delay(t_boucle - (t_fin - t_deb));
		}
    };

	//logger->err("DELAY: %d", delay);
    t_deb = SDL_GetTicks();
}

void* loadMap() {
	changeGameStatus(GAME_LOBY);
}

void launchSate(short status) {
	int i = 0;
	int now = 0;
	Game* game = getGame();
	while(game->status == status) {
	    logger->inf("==== Map Tick #%d ====", i++);
		now = SDL_GetTicks(); 

		render();
    	animate();
    	updatePlayers();

		handleEvents();
    	tickWait(now);

		logger->inf("==== Tick END ====");
	}
}

void renderMap() {
	logger->inf("==== LOADING MAP ====");
	clearObjects();

	AssetMgr* ast = getAssets();
	SDL_Surface* bg = ast->getImg("map");
	addSimpleObject("Background", bg, NULL, 1);

	Player* p = genPlayer("TEST-PLAYER");
	initPlayer(p);

	launchSate(GAME_LOBY);
}

Game* getGame() {
	static Game* game = NULL;
	
	if (game != NULL) {
		return game;
	}

	logger->inf("==== INIT GAME ====");	
	game = malloc(sizeof(game));
	game->status = GAME_MENU;

	return game;
}

void quitGame() {
	logger->inf("==== Quitting Game ====");	
	Game* game = getGame();
	game->status = GAME_QUIT;
}


Button** getMenu() {
	logger->inf("==== Getting Menu ====");	
	
	static Button btnJoin = {
		.z = 2,
		.name = "Find",
		.text = "Find",
		.font = "pf",
		.color = {255,255,255},
		.fontSize = FONT_LG,
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) loadMap,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};
	

	// Define Button Size
	btnJoin.pos.w = LG_BTN_W;
	btnJoin.pos.h = LG_BTN_H;

	// Define Position
	btnJoin.pos.x = SCREEN_W + 5;
	btnJoin.pos.y = percent(45, SCREEN_H);
	
	// Define Animation Position
	btnJoin.anim = malloc(sizeof(AnimParam));
	btnJoin.anim->time = 0.7f;
	btnJoin.anim->delay = 0;
	btnJoin.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnJoin.anim->pos.y = percent(45, SCREEN_H);
	
	btnJoin.anim->pos.w = 0;
	btnJoin.anim->pos.h = 0;
	
	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnJoin.name,
		btnJoin.pos.x,
		btnJoin.pos.y,
		btnJoin.pos.w,
		btnJoin.pos.h,
		btnJoin.anim->pos.x,
		btnJoin.anim->pos.y
	);

	static Button btnHost = {
		.z = 2,
		.name = "Host",
		.text = "Host",
		.font = "pf",
		.color = {255,255,255},
		.fontSize = FONT_LG,
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) sayGoodbye,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};

	// Define Button Size
	btnHost.pos.w = LG_BTN_W;
	btnHost.pos.h = LG_BTN_H;

	// Define Position
	btnHost.pos.x = SCREEN_W + 5;
	btnHost.pos.y = btnJoin.anim->pos.y + (LG_BTN_H  * 1.25);


	// Define Animation Position
	btnHost.anim = malloc(sizeof(AnimParam));
	btnHost.anim->time = 0.7f;
	btnHost.anim->delay = 0.2f;
	btnHost.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnHost.anim->pos.y = btnJoin.anim->pos.y + (LG_BTN_H  * 1.25);
	
	btnHost.anim->pos.w = 0;
	btnHost.anim->pos.h = 0;

	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnHost.name,
		btnHost.pos.x,
		btnHost.pos.y,
		btnHost.pos.w,
		btnHost.pos.h,
		btnHost.anim->pos.x,
		btnHost.anim->pos.y
	);

	static Button btnQuit = {
		.z = 2,
		.name = "Quit",
		.text = "Quit",
		.font = "pf",
		.color = {255,255,255},
		.fontSize = FONT_LG,
		.imgPath = "lg-button-red",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) quitGame,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};

	// Define Button Size
	btnQuit.pos.w = LG_BTN_W;
	btnQuit.pos.h = LG_BTN_H;

	// Define Position
	btnQuit.pos.x = SCREEN_W + 5;
	btnQuit.pos.y = btnHost.anim->pos.y + (LG_BTN_H  * 1.25);



	// Define Animation Position
	btnQuit.anim = malloc(sizeof(AnimParam));
	btnQuit.anim->time = 0.7f;
	btnQuit.anim->delay = 0.4f;
	btnQuit.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnQuit.anim->pos.y = btnHost.anim->pos.y + (LG_BTN_H  * 1.25);

	btnQuit.anim->pos.w = 0;
	btnQuit.anim->pos.h = 0;

	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnQuit.name,
		btnQuit.pos.x,
		btnQuit.pos.y,
		btnQuit.pos.w,
		btnQuit.pos.h,
		btnQuit.anim->pos.x,
		btnQuit.anim->pos.y
	);


	static Button* btns[] = {
		&btnJoin,
		&btnHost,
		&btnQuit,
		NULL
	};

	return btns;
}

void mainMenu() {
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
}

void changeGameStatus(short status) {
	logger->inf("==== Changing Status: %d ====", status);
	Game* game = getGame();

	game->status = status;
}

