#include "main.h"

Log* logger;

void* closeApp() {
	logger->enabled = 1;
	logger->inf("==== Closing App ====");	
	Game* game = getGame();

	// Cleaning Objects	
	logger->dbg("-- Cleaning Players");
	clearPlayers();
	ListManager* players = getPlayerList();
	logger->dbg("-- Deleting Players List");
	free(players);

	// Cleaning Objects	
	logger->dbg("-- Cleaning Objects");
	clearObjects();

	logger->dbg("-- Cleaning Hit Objects");
	ListManager* hitObjects = getHitObjectList();
	deleteList(hitObjects);

	ListManager* objects = getObjectList();
	if (objects != NULL) {
		deleteList(objects);
	}

	AssetMgr* ast = getAssets();
	ast->destroy();

	logger->dbg("-- Quit SDL");
	game->status = GAME_QUIT;
	SDL_Quit();
	
	logger->dbg("-- Free Game");
	free(game);

	logger->dbg("==== Killing Game Proccess ====");
	logger->close;
}

void lockLogger(int flag) {
	Game* game = getGame();

	if (game->flags & flag) {
		pthread_mutex_lock(&logger->mutex);
		//logger->err("Lock");
		//pthread_cond_wait (&logger->cond, &logger->mutex);
		
		logger->enabled = game->flags & flag;
	}
}

void unlockLogger(int flag) {
	Game* game = getGame();

	if (game->flags & flag) {
		logger->enabled = 0;
		//logger->err("Un-Lock");
		//pthread_cond_signal (&logger->cond);
		pthread_mutex_unlock(&logger->mutex);
	}
}

void lock(int flag) {
	Game* game = getGame();

	lockLogger(flag);
	pthread_mutex_lock(&game->mutex);
}

void unlock(int flag) {
	Game* game = getGame();
	unlockLogger(flag);
	pthread_mutex_unlock(&game->mutex);
}

int main(int argc, char *argv[])
{
	logger = initLogger(argc, argv);
	
	logger->inf("##### START GAME #####");

	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	Game* game = getGame();
	parseGameArgs(argc, argv);
	
	logger->inf("-- Init: Window");
	SDL_Surface* screen = getScreen();
	

	logger->enabled = 1;
	logger->war(
		"-- TEST FLAGS: \n--HIT: %d\n--EVNT: %d\n--MOVE: %d\n--VIEW: %d\n--BOMB: %d\n--MOUSE: %d\n--ASSET: %d\n--ANIM: %d\n--MAP: %d\n--OBJ: %d\n--PLAYER: %d\n--STATE: %d\n--MENU: %d",
		game->flags & DBG_HIT,
		game->flags & DBG_EVNT,
		game->flags & DBG_MOVE,
		game->flags & DBG_VIEW,
		game->flags & DBG_BOMB,
		game->flags & DBG_MOUSE,
		game->flags & DBG_ASSET,
		game->flags & DBG_ANIM,
		game->flags & DBG_MAP,
		game->flags & DBG_OBJ,
		game->flags & DBG_PLAYER,
		game->flags & DBG_STATE,
		game->flags & DBG_MENU
	);

	logger->enabled = 0;

	pthread_t renderThread;
	pthread_create (&renderThread, NULL, render, (void*)NULL);
	game->renderThread = &renderThread;

	/*
		int i = 0;
		while(i < 5) {
		    SDL_Delay(1000);

			pthread_mutex_lock(&game->mutex);

		    lockLogger(DBG_VIEW);
		    logger->err("TEST: #%d", i++);
		    unlockLogger(DBG_VIEW);
			
			pthread_mutex_unlock(&game->mutex);
		}

		pthread_mutex_lock(&game->mutex);
		game->status = GAME_QUIT;
		pthread_mutex_unlock(&game->mutex);

		   
		logger->err("==== END TEST ====");

		return 0;
	*/


	//logger->err("INIT: Ask-Lock");
	lock(DBG_STATE);
	//logger->err("INIT: Lock");

	mainMenu();
	while(game->status != GAME_QUIT) {
		switch (game->status) {
			case GAME_MENU:
				mainMenu();
				break;

			case GAME_LOBY:
				renderMap();
				break;

			default:
				break;
		}
	}

	pthread_join (renderThread, NULL);
	closeApp();
}