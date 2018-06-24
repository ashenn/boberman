#include "../server.h"

void init_commands () {
    Game* game = getGame();
    server_t* server = getServer();

    logger->enabled = game->flags & DBG_SERVER;
    logger->inf("==== INIT SERVER COMMANDS ====");
    clearList(server->commands);

    Arg* arg = malloc(sizeof(Arg));
    arg->function = (void*) broadcastMove;
    
    logger->dbg("-- Player Move");
    Node* n = addNodeV(server->commands, "move", arg, 1);



    arg = malloc(sizeof(Arg));    
    arg->function = (void*) broadcastStop;
    
    logger->dbg("-- Player Stop Move");
    n = addNodeV(server->commands, "stopmove", arg, 1);



    arg = malloc(sizeof(Arg));
    arg->function = (void*) placeBomb;
    
    logger->dbg("-- Place Bomb");
    n = addNodeV(server->commands, "bomb", arg, 1);
    
    logger->dbg("==== INIT SERVER COMMANDS DONE ====");
}