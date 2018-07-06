#include "server.h"

client_t* getClient() {
	server_t* server = getServer();
	if(server == NULL) {
		return NULL;
	}

	Node* node = server->clients->first;
	if (node == NULL) {
		return NULL;
	}

	return node->value;
}

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

void closeServer() {
	logger->inf("===== Closing Server =====");
	
	server_t* server = getServer();
	if(server == NULL) {
		logger->dbg("Nothing to do");
		return;
	}

	logger->dbg("deleting Commands");
	deleteList(server->commands);

	logger->dbg("Cleaning Clients");
	Node* n = NULL;

	while((n = listIterate(server->clients, n))) {
	    client_t* cli = n->value;
	    free(cli->name);
	}

	deleteList(server->clients);
	
	logger->dbg("Closing Socket");
	close(server->fd);
	
	logger->dbg("Free Server");
	free(server);
}

void* serverProcess() {
	logger->err("============== LAUNCH SERVER =============");
	Game* game = getGame();

	logger->inf("==== LAUNCHING SERVER THREAD ====");

	//logger->err("-- Server Ask-Lock");
	lock(DBG_SERVER);
	//logger->war("-- Server Lock");

	server_t* server = initServer(1);

	//logger->err("-- Server Trigger Signal !!!");
	signalCond();
	if(server == NULL) {
		logger->war("-- Server Unlock FAILD INIT");
		unlock(DBG_SERVER);
		return NULL;
	}


	if(server->fd < 0) {
		logger->war("-- Server Unlock FAILD SOCKET");
		unlock(DBG_SERVER);
		return NULL;
	}

	while(game->status <= GAME_END) {
		//logger->err("-- Server Unlock");
		unlock(DBG_SERVER);

	    network_handling(server->fd, server->addr);
	    //usleep(5);

		//logger->err("-- Server Ask-Lock");
		lock(DBG_SERVER);
		//logger->war("-- Server Lock");
	}


	logger->dbg("==== SERVER THREAD END ====");
	//logger->err("-- Server Unlock");
	unlock(DBG_SERVER);
	pthread_exit(NULL);
}