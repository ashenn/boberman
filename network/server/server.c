#include "server.h"


server_t* initServer(short init) {
	static server_t* server = NULL;
	
	if (!init || server != NULL) {
		return server;
	}

	Game* game = getGame();
	logger->enabled = game->flags & DBG_SERVER;
	logger->inf("==== INIT SERVER ====");
	
	server = socket_init(inet_addr("127.0.0.1"), game->options.port);
	if(server == NULL) {
		return NULL;
	}

	logger->dbg("-- init Commands List");
	server->commands = initListMgr();
	
	logger->dbg("-- init Clients List");
	server->clients = initListMgr();
	init_commands();
	return server;
	
}

server_t* getServer() {
	return initServer(0);
}

void* serverProcess() {
	Game* game = getGame();
	
	logger->inf("==== LAUNCHING SERVER THREAD ====");
	
	//logger->dbg("-- Server Ask-Lock");
	lock(DBG_SERVER);
	//logger->dbg("-- Server Locked");

	logger->dbg("-- Server INIT");
	server_t* server = initServer(1);
	if(server == NULL) {
		return NULL;
	}
	
	signalCond();

	if(server->fd < 0) {
		logger->err("Faild To Init Socket !!!");
		unlock(DBG_SERVER);
		return NULL;
	}

	while(game->status < GAME_END) {
		//logger->dbg("-- Server Unlock");
		unlock(DBG_SERVER);
	    network_handling(server->fd, server->addr);
		
	    //usleep(5);
	    
		//logger->dbg("-- Server Ask-Lock");
		lock(DBG_SERVER);
		//logger->dbg("-- Server Locked");
	}

	logger->dbg("-- Free Server");
	deleteList(server->clients);
	deleteList(server->commands);

	close(server->fd);
	free(server);

	logger->dbg("==== SERVER THREAD END ====");
	unlock(DBG_SERVER);
}