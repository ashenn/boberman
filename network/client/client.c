#include "client.h"

Connexion* initConnexion(short init) {
	static Connexion connexion = {
		.id = 0,
		.fd = 0,
		.init = 0,
		.name = NULL
	};

	if(!init || connexion.init) {
		return &connexion;
	}

	Game* game = getGame();
	enableLogger(DBG_CLIENT);

	logger->inf("==== Setting Client Connexion ====");
	logger->dbg("-- Client Socket");
	connexion.fd = socket(AF_INET, SOCK_STREAM, 0);

	connexion.client.sin_family = AF_INET;
	connexion.client.sin_port = 0;
	connexion.client.sin_addr.s_addr = inet_addr("localhost");

	bind(connexion.fd, (struct sockaddr *) &connexion.client, sizeof(connexion.client));


	logger->dbg("-- Server Socket");
	connexion.server.sin_family = AF_INET;
	connexion.server.sin_port = htons(game->options.port);
	connexion.server.sin_addr.s_addr = inet_addr(game->options.ip);


	logger->dbg("-- Connecting To Server");
	if (connect(connexion.fd, (struct sockaddr *) &connexion.server, sizeof(connexion.server)) == -1) {
	    logger->err("Faild To Connect To Server");
	    return NULL;
	}

	connexion.init = 1;

	logger->dbg("-- Connection SUCCESS");
	return &connexion;
}

Connexion* getConnexion() {
	return initConnexion(0);
}

void getMessage(char* msg) {
	logger->err("Message recieved !!!");
	memset(msg, 0, MSG_SIZE);
	Connexion* co = getConnexion();


	int i = recv(co->fd, msg, 1, 0);
	logger->err("Test Décode !!! %d", (int)msg[0]);

	i = recv(co->fd, msg, (int)msg[0] +1, 0);
	logger->err("Test MSG !!! %s", msg);

	if(i < 0) {
		logger->err("Faild To Receive Message !!!");
	}
}

short findHost() {
	enableLogger(DBG_CLIENT);
	logger->inf("==== Findding Host ====");

	Connexion* co = initConnexion(1);

	if(co == NULL || !co->init) {
		return 0;
	}

	char msg[MSG_SIZE];
	logger->inf("-- Checking Response");

	unlock(DBG_CLIENT);
	getMessage(msg);
	logger->inf("-- msg: %s");

	char* resp[3];
	explode(':', msg, 0, 0, resp);

	logger->inf("-- Checking explode");
	logger->inf("-- 0: %s\n--1: %s", resp[0], resp[1]);

	int id = str2int(resp[1]);

	free(resp[0]);
	free(resp[1]);

	if(!id) {
		logger->err("Connexion Refuse !!!");
		return 0;
	}

	if(getServer() == NULL) {
		lock(DBG_STATE);
		clearObjects();
		unlock(DBG_STATE);

		char name[12];
		memset(name, 0, 12);
		for(int i = 1; i < id; i++) {
			Player* p = genPlayer("Player-1");
		}
		snprintf(name, 12, "player-%d", id);
		initPlayer(genPlayer(name));
	}

	logger->inf("Connexion Accepted");

	return 1;
}


void* clientProcess() {
	enableLogger(DBG_CLIENT);
	logger->inf("==== INIT CLIENT THREAD ====");


	Game* game = getGame();
	char msg[MSG_SIZE];

	fd_set readfds;
	struct timeval tv = {0, 20};
	Connexion* co = getConnexion();
  	lock(DBG_CLIENT);

	while(game->status < GAME_QUIT) {

		logger->err("Client: Un-Lock");
	    unlock(DBG_CLIENT);

		enableLogger(DBG_CLIENT);

		logger->dbg("-- Resetting fd");

		FD_ZERO(&readfds);
		FD_SET(0, &readfds);
		FD_SET(co->fd, &readfds);
		int max_sd = co->fd;

		enableLogger(DBG_CLIENT);
		logger->dbg("-- Getting Activity");
		int activity = select(max_sd + 1, &readfds, NULL, NULL, &tv);

		if ((activity < 0) && (errno != EINTR)) {
			enableLogger(DBG_CLIENT);

		    logger->war("No Activity");

			//logger->war("Client: Ask-Lock");
	    	lock(DBG_CLIENT);
			//logger->war("Client: Lock");
		    continue;
		}

		enableLogger(DBG_CLIENT);
		logger->dbg("-- Checking fd");
		if (!FD_ISSET(co->fd, &readfds)) {
			enableLogger(DBG_CLIENT);

			logger->dbg("-- read fd is empty skipping");
			//logger->war("Client: Ask-Lock");

	    	lock(DBG_CLIENT);
			//logger->war("Client: Lock");
		    continue;
		}

		enableLogger(DBG_CLIENT);
		logger->err("-- Getting Message");
	  	getMessage(msg);
		logger->err("-- Got Message %s\nMsg Length:%d", msg, strlen(msg));

	    if(strlen(msg) == 0) {
			enableLogger(DBG_CLIENT);
			logger->err("-- msg is empty continue...");
			//logger->war("Client: Ask-Lock");

	    	lock(DBG_CLIENT);
			//logger->war("Client: Lock");
	    	continue;
	    }

		enableLogger(DBG_CLIENT);
		logger->err("-- Msg: %s", msg);

		char* resp[3];
		explode(':', msg, 0, 0, resp);
		Player *pl = getPlayer();
		logger->dbg("%d", pl->id);

		if (strcmp(resp[0], "newPlayer") == 0) {
			logger->err("New player Recieved : %s (I am %d)", resp[1], pl->id);
			if (pl->id != str2int(resp[1]))
				genPlayer("Player-n");
		}

		if (strcmp(resp[0], "playerLeft") == 0) {
			logger->err("Player left Recieved : %s (I am %d)", resp[1], pl->id);
			ListManager* players = getPlayerList();
			Node *tmp = NULL;
			while((tmp = listIterate(players, tmp)) != NULL) {
				Player *deadPlayer = tmp->value;
				if (deadPlayer->id == str2int(resp[1]))
					killPlayer(deadPlayer);
			}
		}

		if (strcmp(resp[0], "playerkilled") == 0) {
			logger->err("player killed Recieved : %s (I am %d)", resp[1], pl->id);
			ListManager* players = getPlayerList();
			Node *tmp = NULL;
			while((tmp = listIterate(players, tmp)) != NULL) {
				Player *deadPlayer = tmp->value;
				if (deadPlayer->id == str2int(resp[1]))
					killPlayer(deadPlayer);
			}
		}

		if (strcmp(resp[0], "breackblock") == 0) {
			logger->err("Break Block By Id: %d", resp[1]);
			ListManager* objs = getObjectList();

			int id = str2int(resp[1]);
			Node* blockNode = getNode(objs, id);

			if(blockNode != NULL) {
				Object* o = blockNode->value;

				logger->err("Object Got: %s", o->name);

				if(o->containerType == BLOCK) {
					breakBlock(o->container);
					SDL_Rect bonuPos;
					Block *block = o->container;
					bonuPos.x = block->obj->pos.x + (BONUS_SIZE / 4);
					bonuPos.y = block->obj->pos.y + (BONUS_SIZE / 4);

					bonuPos.w = BONUS_SIZE;
					bonuPos.h = BONUS_SIZE;
					generateBonus(bonuPos, resp[2]);
				}
			}
		}

		if (strcmp(resp[0], "bombPlaced") == 0) {
			logger->err("Bomb Placed By Id: %d", resp[1]);
			int id = str2int(resp[1]);
			ListManager* players = getPlayerList();
			Node* playernode = getNode(players, id);


			if(playernode != NULL) {
					Player *player = playernode->value;
					if(player != NULL) {
						placeBomb(player);
					}
			}
		}


		//logger->war("Client: Ask-Lock");
	    lock(DBG_CLIENT);
		//logger->war("Client: Lock");
	}

	close(co->fd);

	//logger->war("Client: Un-Lock");
    unlock(DBG_CLIENT);
}

void sendCommand(char* cmd, int val) {
	Player* p = getPlayer();
	if(p == NULL) {
		logger->err("Can't Send command If player is not init");
		return;
	}

	Connexion* co = getConnexion();

	char msg[MSG_SIZE];
	memset(msg, 0, MSG_SIZE);
	snprintf(msg, MSG_SIZE, "%s:%d", cmd, val);

	logger->err("#### Sending Comand %s", msg);
	send(co->fd, msg, MSG_SIZE, 0);
}
