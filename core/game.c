#include <sys/time.h>
#include "game.h"

void timer() {
	Game* game = getGame();
	if(game->status >= GAME_TIMEOUT) {
		return;
	}

	static short timeleft = 90;
	static Object* curTxt = NULL;
	char msg[25];
	memset(msg, 0, 25);

	if(game->status >= GAME_END) {
		if(curTxt != NULL) {
			curTxt->lifetime = 3;
			curTxt->onDelete = NULL;
		}
		return;
	}

	if(--timeleft > 0) {
		snprintf(msg, 25, "%d:%02d", timeleft / 60, timeleft % 60);

		Object* txt = generateText(msg, "pf", FONT_LG);
		txt->lifetime = 3;
		txt->onDelete = timer;
		curTxt = txt;
	//	broadcast(msg, getClient());
	}
	else {
		changeGameStatus(GAME_TIMEOUT);
	}
}

void countDown() {
	static short cnt = 5;

	char msg[25];
	memset(msg, 0, 25);

	Game* game = getGame();
	if(game->status >= GAME_RUNNING) {
		return;
	}
	else if(cnt == -1) {
		changeGameStatus(GAME_RUNNING);
		//snprintf(msg, 25, "GO !!!");
	}
	else if(cnt < -1) {
		return;
	}
	else {
		snprintf(msg, 25, "%d", cnt);
		Object* txt = generateText(msg, "pf", FONT_LG);
		txt->lifetime = 3;
		txt->onDelete = countDown;
	}
	cnt--;
}

void startGame() {
	Object* txt = generateText("Get Ready !", "pf", FONT_LG);
	txt->lifetime = 5;
	txt->onDelete = countDown;

	launchSate(GAME_START);
}

void* setServerPort(int port) {
	Game* game = getGame();
	game->options.port = port;
}

void* setServerIp(char* ip) {
	Game* game = getGame();
	strcpy(game->options.ip, ip);
}

void* findGame() {
	Game* game = getGame();
	enableLogger(DBG_CLIENT);

	logger->inf("==== FIND GAME ====");
	Connexion* co = getConnexion();

	if(co->init) {
		logger->war("GAME ALREADY INIT");
		changeGameStatus(GAME_MENU);
		return NULL;
	}

	if(!findHost()) {
		logger->war("Faild To Find Host !!!");
		changeGameStatus(GAME_MENU);
		return NULL;
	}

	logger->war("Host Found Host !!!");

	pthread_create(&game->clientThread, NULL, clientProcess, (void*)NULL);
	logger->dbg("clientThread created");

	loadMap();
}

void* hostGame() {
	Game* game = getGame();
	server_t* serv = getServer();

	if(serv != NULL) {
		return NULL;
	}

	AssetMgr* ast = getAssets();

	//logger->war("Host: Un-lock");
	unlock(DBG_SERVER);
	//logger->war("Host: Test");

	pthread_create(&game->serverThread, NULL, serverProcess, (void*)NULL);

	//logger->war("Host: WAIT !!!");
	lock(DBG_SERVER);
	waitCond();
	unlock(DBG_SERVER);
	//logger->war("Host: WAIT DONE !!!");

	//logger->war("Host: Ask-lock");
	lock(DBG_SERVER);
	//logger->war("Host: lock");

	serv = getServer();

	if(serv == NULL || serv->fd < 0) {
		unlock(DBG_SERVER);
		logger->err("-- faild");
		return NULL;
	}

	game->isServer = 1;
	clearObjects();
	logger->err("==== CLEARING IMAGES ====");
	ast->clearImgs();
	setServerIp("127.0.0.1");

	/*
	logger->war("Host: Un-lock");
	unlock(DBG_SERVER);
	*/

	loadMap();
	findGame();

	//logger->war("Host: Ask-lock");
	lock(DBG_SERVER);
	//logger->war("Host: lock");
	return NULL;
}

void refreshPlayers() {
	Node* n = NULL;
	ListManager* players = getPlayerList();

	char tmp[15];

	char msg[50];

	memset(msg, 0, 50);
	snprintf(msg, 40, "refresh:");
	while((n = listIterate(players, n)) != NULL) {
		Player* p = n->value;

		memset(tmp, 0, 15);
	    snprintf(tmp, 15, "%d-%d-%d", p->id, p->object->pos.x, p->object->pos.y);

	    strcat(msg, tmp);
	    if(n != players->last) {
	    	strcat(msg, ":");
	    }
	}

	//logger->war("##### REFRESH PLAYERS ####");
	//logger->war("msg: %s", msg);

	broadcast(msg, getClient());
}

void tick() {
	Game* game = getGame();

	if (game->status < GAME_LOBY) {
		return;
	}

	static int cnt = 0;

	if(++cnt == TICK_REFRESH) {
		resetPlayersBomb();
		clearOutdatedObjects();

		if(game->isServer) {
			refreshPlayers();
		}

		cnt = 0;
	}
}

void tickWait(int next) {
	int now = SDL_GetTicks();
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
	game->status = status;

	/*
		logger->err("GAME: Un-Lock");
		unlock(DBG_STATE);
	*/

	while(game->status == status) {

		logger->enabled = game->flags & DBG_STATE;

		now = SDL_GetTicks();
		add = (int)(1000 / (float)FPS);

	    nextFrame = now + add;

		handleEvents();

    	if (game->isServer && game->status > GAME_LOBY && game->status <= GAME_RUNNING) {
    		handleHits();
    	}

		logger->enabled = game->flags & DBG_STATE;
		logger->inf("==== Tick END ====");


		//logger->war("GAME: Un-Lock");
		unlock(DBG_STATE);

		SDL_Delay(15);

		//logger->war("GAME: Ask-Lock");
		lock(DBG_STATE);
		//logger->war("GAME: Lock");
	}

	logger->dbg("===== STATE END =====");
}

void renderMap() {

	/*
		logger->war("MAP: Ask-Lock");
		lock(DBG_STATE);
		logger->war("MAP: Lock");
	*/

	Game* game = getGame();

	logger->enabled = game->flags & DBG_MAP;
	AssetMgr* ast = getAssets();

	logger->inf("==== LOADING MAP ====");
	SDL_Surface* bg = ast->getImg("map");
	addSimpleObject("Background", bg, NULL, 1);
	

	generateWalls();

	launchSate(GAME_LOBY);
}

void initGameFlags(Game* game) {
	game->flagList = initListMgr();

	static int flags[16] = {
		DBG_HIT,
		DBG_EVNT,
		DBG_MOVE,
		DBG_VIEW,
		DBG_BOMB,
		DBG_MOUSE,
		DBG_ASSET,
		DBG_ANIM,
		DBG_MAP,
		DBG_OBJ,
		DBG_PLAYER,
		DBG_STATE,
		DBG_MENU,
		DBG_SERVER,
		DBG_CLIENT
	};



	addNodeV(game->flagList, "hit", flags + 0, 0);
	addNodeV(game->flagList, "evnt", flags + 1, 0);
	addNodeV(game->flagList, "move", flags + 2, 0);
	addNodeV(game->flagList, "view", flags + 3, 0);
	addNodeV(game->flagList, "bomb", flags + 3, 0);
	addNodeV(game->flagList, "mouse", flags + 5, 0);
	addNodeV(game->flagList, "asset", flags + 6, 0);
	addNodeV(game->flagList, "anim", flags + 7, 0);
	addNodeV(game->flagList, "map", flags + 8, 0);
	addNodeV(game->flagList, "obj", flags + 6, 0);
	addNodeV(game->flagList, "player", flags + 10, 0);
	addNodeV(game->flagList, "state", flags + 11, 0);
	addNodeV(game->flagList, "menu", flags + 12, 0);
	addNodeV(game->flagList, "server", flags + 13, 0);
	addNodeV(game->flagList, "client", flags + 14, 0);

}

Game* getGame() {
	static Game* game = NULL;

	if (game != NULL) {
		return game;
	}

	logger->enabled = 1;
	logger->inf("==== INIT Game* game ====");
	game = malloc(sizeof(Game));

	game->isServer = 0;
	game->maxPlayer = 2;
	game->flags = NO_FLAG;
	game->status = GAME_MENU;
	initGameFlags(game);

	game->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	game->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

	memset(game->options.ip, 0, sizeof(game->options.ip));
	game->options.port = DEFAULT_PORT;

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
		btnJoin->click = (void*) findGame;
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
		btnHost->click = (void*) hostGame;
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
		btnHost->anim->time = 0.1f;
		btnHost->anim->delay = 0.0f;
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

	if(game->isServer) {
		char msg[10];
		memset(msg, 0, 10);
		snprintf(msg, 10, "status:%d", status);
		broadcast(msg, getClient());
	}
}

void* addDebugFlag(char* flag) {
	Game* game = getGame();

	logger->err("==== setting Flag: %s ====", flag);
	Node* n = getNodeByName(game->flagList, flag);

	if(n == NULL) {
		logger->err("Faild To Found Flag: %s", flag);
		return NULL;
	}

	game->flags = game->flags | *((int*) n->value);
}

void* setMaxPlayer(int max) {
	if(max < 2 || max > 4) {
		logger->war("Bad Value For Max Player: %d", max);
		logger->war("Max Player Set to 4");

		max = 4;
	}

	Game* game = getGame();
	game->maxPlayer = max;
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

	static Arg arg2 = {
		.name = "p",
		.function = setServerPort,
		.hasParam = 1,
		.defParam = NULL,
		.asInt = 1,
		.type="num"
	};

	static Arg arg3 = {
		.name = "ip",
		.function = setServerIp,
		.hasParam = 1,
		.defParam = NULL,
		.asInt = 0,
		.type="any"
	};

	static Arg arg4 = {
		.name = "n",
		.function = setMaxPlayer,
		.hasParam = 1,
		.defParam = NULL,
		.asInt = 1,
		.type="num"
	};

	static  Arg* args[] = {
		&arg1,
		&arg2,
		&arg3,
		&arg4,
		NULL
	};

	ListManager* lst = defineArgs(args);
	parseArgs(lst, argc, argv);
	deleteList(lst);
}
