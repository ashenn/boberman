#include "../server.h"

void init_commands () {
    Game* game = getGame();
    server_t* server = getServer();

    logger->enabled = game->flags & DBG_SERVER;
    logger->inf("==== INIT SERVER COMMANDS ====");
    clearList(server->commands);

    logger->dbg("-- Player Move");
    addNodeV(server->commands, "move", &playerMove, 0);
    
    logger->dbg("-- Place Bomb");
    addNodeV(server->commands, "bomb", &placeBomb, 0);
    
    logger->dbg("==== INIT SERVER COMMANDS DONE ====");
}