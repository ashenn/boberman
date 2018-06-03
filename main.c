#include "main.h"

Log* logger;

void* closeApp() {
	logger->inf("==== Closing App ====");	
	Game* game = getGame();

	// Cleaning Objects	
	logger->dbg("-- Cleaning Players");
	clearPlayers();
	ListManager* players = getPlayerList();
	free(players);


	ListManager* hitObjects = getHitObjectList();
	logger->dbg("-- Cleaning Hit Objects");
	deleteList(hitObjects);

	// Cleaning Objects	
	logger->dbg("-- Cleaning Objects");
	clearObjects();

	ListManager* objects = getObjectList();
	if (objects != NULL) {
		deleteList(objects);
	}

	AssetMgr* ast = getAssets();
	ast->destroy();

	logger->dbg("-- Quit SDL");
	game->status = GAME_QUIT;
	SDL_Quit();
	
	logger->dbg("==== Killing Game Proccess ====");
}

int main(int argc, char *argv[])
{
	logger = initLogger(argc, argv);
	//logger->inf("=== Starting App ===");

	//logger->inf("-- Init: SDL");
	TTF_Init();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	//logger->inf("-- Init: Window");
	SDL_Surface* screen = getScreen();

	Game* game = getGame();
	game->flags = DBG_HIT;

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

		handleEvents();
	}

	closeApp();
}