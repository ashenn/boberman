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
    	handleHits();

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
	
	Player* p2 = genPlayer("TEST-Hit Player");
	p2->pos.y = 0;
	p2->pos.x = 12;

	moveTo(p2->object, CELL_SIZE * 12, CELL_SIZE, 0.3, 0);

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
	btnHost->pos.y = btnJoin->anim->pos.y + (LG_BTN_H  * 1.25);


	// Define Animation Position
	btnHost->anim = malloc(sizeof(AnimParam));
	btnHost->anim->time = 0.7f;
	btnHost->anim->delay = 0.2f;
	btnHost->anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
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
	btnQuit->pos.y = btnHost->anim->pos.y + (LG_BTN_H  * 1.25);



	// Define Animation Position
	btnQuit->anim = malloc(sizeof(AnimParam));
	btnQuit->anim->time = 0.7f;
	btnQuit->anim->delay = 0.4f;
	btnQuit->anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
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
	logger->inf("==== Changing Status: %d ====", status);
	Game* game = getGame();

	game->status = status;
}

