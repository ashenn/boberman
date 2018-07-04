#include "main.h"
#include "network/server/server.h"
#include "network/client/client.h"

Log* logger;

void enableLogger(int flag) {
	Game* game = getGame();
	logger->enabled = game->flags & flag;
}

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

	logger->dbg("-- Cleaning Bonus");
	ListManager* bonusList = getBonusList();
	deleteList(bonusList);

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
	deleteList(game->flagList);
	free(game);

	logger->dbg("==== Killing Game Proccess ====");
	logger->close;
}

void lockLogger(int flag) {
	Game* game = getGame();

	if (game->flags & flag) {
		logger->enabled = game->flags & flag;
	}
}

void unlockLogger(int flag) {
	Game* game = getGame();

	if (game->flags & flag) {
		logger->enabled = 0;
	}
}

void lock(int flag) {
	Game* game = getGame();

	pthread_mutex_lock(&game->mutex);
	lockLogger(flag);
}

void unlock(int flag) {
	Game* game = getGame();
	unlockLogger(flag);
	pthread_mutex_unlock(&game->mutex);
}

void waitCond() {
	Game* game = getGame();
	pthread_cond_wait(&game->cond, &game->mutex);
}

void signalCond() {
	Game* game = getGame();
	pthread_cond_signal(&game->cond);
}

void gameOver() {
	char msg[45];
		memset(msg, 0, 45);
	snprintf(msg, 45, "TIME EXPIRED : GAME OVER");

	Object* txt = generateText(msg, "pf", FONT_LG);
	txt->lifetime = -1;
}

void getWinner() {
	logger->war("=== GETTING WINNER ===");
	Node* n = NULL;
	ListManager* players = getPlayerList();

	Player* p = NULL;
	Player* winner = NULL;
	while((n = listIterate(players, n)) != NULL) {
		p = n->value;
		logger->war("#%d: %s => %d", p->id, p->name, p->alive);

	    if(p->alive) {
			logger->err("Found Alive: #%d: %s => %d !!!!", p->id, p->name, p->alive);
	    	winner = p;
	    	break;
	    }
	}

		char msg[45];
		memset(msg, 0, 45);
		if(winner != NULL) {
			snprintf(msg, 45, "THE WINNER IS %s !!!!", winner->name);
		}
		else {
			snprintf(msg, 45, "EVERYBODY IS DEAD LOOSERS !!!!");
		}

	Object* txt = generateText(msg, "pf", FONT_LG);
	txt->lifetime = -1;
	logger->inf("Winner Msg: \n %s", msg);
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

	if(!game->options.ip[0]) {
		logger->war("Setting Default IP");
		snprintf(game->options.ip, 16, "127.0.0.1");
	}

	logger->enabled = 1;
	logger->war("Server Address: %s:%d", game->options.ip, game->options.port);

	logger->war(
		"-- TEST FLAGS: \n--HIT: %d\n--EVNT: %d\n--MOVE: %d\n--VIEW: %d\n--BOMB: %d\n--MOUSE: %d\n--ASSET: %d\n--ANIM: %d\n--MAP: %d\n--OBJ: %d\n--PLAYER: %d\n--STATE: %d\n--MENU: %d\n--BONUS: %d\n--SERVER: %d\n--CLIENT: %d",
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
		game->flags & DBG_MENU,
		game->flags & DBG_BONUS,
		game->flags & DBG_SERVER,
		game->flags & DBG_CLIENT
	);

	logger->enabled = 0;

	pthread_create (&game->renderThread, NULL, render, (void*)NULL);

	//logger->war("INIT: Ask-Lock");
	lock(DBG_STATE);
	//logger->war("INIT: Lock");

	mainMenu();
	while(game->status != GAME_QUIT) {
		switch (game->status) {
			case GAME_MENU:
				mainMenu();
				break;

			case GAME_LOBY:
				renderMap();
				break;

			case GAME_START:
				logger->err("#### STARTING GAME ####");
				startGame();
				break;

			case GAME_RUNNING:
				logger->err("#### RUNNING GAME ####");
				timer();
				launchSate(GAME_RUNNING);
				break;

		  case GAME_TIMEOUT:
				logger->err("#### GAME TIMEOUT ####");
				gameOver();
				launchSate(GAME_TIMEOUT);
				break;

			case GAME_END:
				logger->err("#### END GAME ####");
				getWinner();
				launchSate(GAME_END);
				break;
		}
	}

	//logger->err("GAME: Un-Lock");
	game->status = GAME_QUIT;
	unlock(DBG_STATE);
	logger->err("WAINTING FOR THREADS END");

	logger->err("JOIN RENDER");
	pthread_join(game->renderThread, NULL);

	if(getServer() != NULL) {
		logger->err("JOIN SERVER");
		pthread_join(game->serverThread, NULL);
	}

	Connexion* co = getConnexion();
	if(co->init) {
		logger->err("JOIN CLIENT");
		pthread_join(game->clientThread, NULL);
	}

	logger->err("THREADS ENDED");
	closeApp();
}
