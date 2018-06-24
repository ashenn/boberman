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
	logger->war("Test MSG !!! %s", msg);

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

	logger->err("-- Checking explode");
	logger->err("-- 0: %s\n--1: %s", resp[0], resp[1]);

	printf("%s\n", resp[1]);
	int id = char2int(resp[1][0]);
	logger->err("id : %d", id);

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

		//logger->err("Client: Un-Lock");
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

		char* resp[5];
		resp[4] = '\0';

		explode(':', msg, 0, 0, resp);
		Player *pl = getPlayer();
		logger->dbg("%d", pl->id);

		//logger->war("Client: Ask-Lock");
	    lock(DBG_CLIENT);
		//logger->war("Client: Lock");

		if (strcmp(resp[0], "newPlayer") == 0) {
			logger->err("New player Recieved : %s (I am %d)", resp[1], pl->id);
			if (pl->id != str2int(resp[1]))
				genPlayer("Player-n");
		}

		if (strcmp(resp[0], "playerLeft") == 0) {
			logger->dbg("Player left Recieved : %s (I am %d)", resp[1], pl->id);
			ListManager* players = getPlayerList();
			Node *tmp = NULL;
			while((tmp = listIterate(players, tmp)) != NULL) {
				Player *deadPlayer = tmp->value;
				if (deadPlayer->id == str2int(resp[1]))
					killPlayer(deadPlayer);
			}
		}

		if (strcmp(resp[0], "playerkilled") == 0) {
			logger->dbg("player killed Recieved : %s (I am %d)", resp[1], pl->id);
			ListManager* players = getPlayerList();
			Node *tmp = NULL;
			while((tmp = listIterate(players, tmp)) != NULL) {
				Player *deadPlayer = tmp->value;
				if (deadPlayer->id == str2int(resp[1]))
					killPlayer(deadPlayer);
			}
		}

		if (strcmp(resp[0], "breackblock") == 0) {
			logger->dbg("Break Block By Id: %d", resp[1]);
			ListManager* blockList = getBlockList();

			int id = str2int(resp[1]);
			Node* blockNode = getNode(blockList, id);

			if(blockNode != NULL) {
				Block* block = blockNode->value;
				Object* o = block->obj;

				logger->dbg("Object Got: %s", o->name);

				breakBlock(block);
				SDL_Rect bonuPos;

				bonuPos.x = block->obj->pos.x + (BONUS_SIZE / 4);
				bonuPos.y = block->obj->pos.y + (BONUS_SIZE / 4);

				bonuPos.w = BONUS_SIZE;
				bonuPos.h = BONUS_SIZE;

				int bonusType = str2int(resp[2]);


				generateBonus(bonuPos, bonusType);
				deleteNode(blockList, block->id);
			}
		}

		if (strcmp(resp[0], "bombPlaced") == 0) {
			logger->dbg("Bomb Placed By Id: %d", resp[1]);
			int id = str2int(resp[1]);
			ListManager* players = getPlayerList();
			Node* playerNode = getNode(players, id);


			if(playerNode != NULL) {
					Player *player = playerNode->value;
					if(player != NULL) {
						placeBomb(player);
					}
			}
		}


		if (strcmp(resp[0], "move") == 0) {
			int id = str2int(resp[1]);
			int direction = str2int(resp[2]);
			logger->dbg("Moving Player #%d: %d", id, direction);

			ListManager* players = getPlayerList();
			Node* playerNode = getNode(players, id);

			if(playerNode != NULL) {
				Player *player = playerNode->value;
				if(player != NULL) {

					playerMove(player, direction);
				}
			}
		}

		if (strcmp(resp[0], "stop") == 0) {
			logger->dbg("Stopping Player #%d", resp[1]);
			int id = str2int(resp[1]);

			ListManager* players = getPlayerList();
			Node* playerNode = getNode(players, id);

			if(playerNode != NULL) {
				Player *player = playerNode->value;
				if(player != NULL) {
					int direction = str2int(resp[1]);

					playerStop(player);
				}
			}
		}

		if (strcmp(resp[0], "status") == 0) {
			int status = str2int(resp[1]);
			logger->war("CHANGING STAUS %d", status);
			changeGameStatus(status);
		}

		if (strcmp(resp[0], "bonus") == 0) {
			int bId = str2int(resp[1]);
			int pId = str2int(resp[2]);

			logger->dbg("Bonus #%d Taken By Player #%d", bId, pId);


			ListManager* bonusList = getBonusList();
			logger->dbg("### PRINTING BONUS LIST");
			printNodes(bonusList);

			Node* bonusNode = getNode(bonusList, bId);

			if(bonusNode != NULL) {
				ListManager* players = getPlayerList();
				Node* playerNode = getNode(players, pId);

				if(playerNode != NULL) {
					Player* player = playerNode->value;
					
					if(player != NULL) {
						Bonus* bonus = bonusNode->value;
						logger->dbg("### Applying Bonus");
						bonus->obj->collision->fnc(bonus->obj, player->object);
						
						logger->dbg("### Deleting Bonus");
						deleteObject(bonus->obj);
					}
					else{
						logger->dbg("### PLAYER IS NULL !!!!");
					}
				}
				else{
					logger->dbg("### PLAYER NOT FOUND !!!!");
				}

				deleteNode(bonusList, bonusNode->id);
			}
			else{
				logger->dbg("### BONUS NOT FOUND !!!!");
			}
		}


		if (strcmp(resp[0], "refresh") == 0) {
			logger->dbg("#### Refreshing Players");

			ListManager* players = getPlayerList();
			int index;
			for (index = 1; index < 4 && resp[index] != '\0'; ++index) {
				logger->dbg("#%d: %s", index, resp[index]);

				char* infos[4];
				infos[3] = '\0';

				explode('-', resp[index], 0, 0, infos);
				int pId = str2int(infos[0]);
				int x = str2int(infos[1]);
				int y = str2int(infos[2]);

				logger->dbg("Player #%d\nPosition: %d | %d", pId, x, y);
				Node* n = getNode(players, pId);
				if(n != NULL) {
					logger->dbg("Setting Pos: %d | %d", x, y);
					Player* p = n->value;
					p->object->pos.x = x;
					p->object->pos.y = y;

					logger->dbg("Player Moved Pos: %d | %d", x, y);
				}

				logger->dbg("Free Params");
				free(infos[0]);
				free(infos[1]);
				free(infos[2]);

			}
		}
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
