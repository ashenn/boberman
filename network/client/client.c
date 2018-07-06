#include "client.h"

void cmdRefresh(char* resp[]) {
	//logger->err("#### Refreshing Players");

	//logger->war("#### Getting Players");
	ListManager* players = getPlayerList();
	//logger->war("#### Players Count: %d", players->nodeCount);
	//logger->war("#### TEST");
	//logger->war("#### %p", resp[0]);
	//logger->war("#### TEST2");
	//logger->war("#### %p", resp[1]);
	//logger->war("#### TEST3");

	int index;
	for (index = 1; index < 4 && resp[index] != '\0'; ++index) {
		//logger->war("#%d", index);
		//logger->war("%s", resp[index]);

		char* infos[4];
		memset(infos, 0, 4);

		explode('-', resp[index], 0, 0, infos);
		int pId = str2int(infos[0]);
		int x = str2int(infos[1]);
		int y = str2int(infos[2]);

		//logger->war("Player #%d\nPosition: %d | %d", pId, x, y);
		Node* n = getNode(players, pId);
		if(n != NULL) {
			//logger->war("Setting Pos: %d | %d", x, y);
			Player* p = n->value;
			p->object->pos.x = x;
			p->object->pos.y = y;

			//logger->war("Player Moved Pos: %d | %d", x, y);
		}

		//logger->war("Free Params");
		int z;
		for (z = 0; infos[z] != 0; ++z){
			free(infos[z]);
			infos[z] = 0;
		}
		memset(infos, 0, 4);
	}
	
	//logger->err("#### REFRESH DONE");
}

void cmdNewPlayer(int id) {
	Player* curPlayer = getPlayer();
	logger->inf("New player Recieved : %d (I am %d)", id, curPlayer->id);

	if (curPlayer->id == id) {
		return;
	}

	char name[15];
	memset(name, 0, 15);
	snprintf(name, 15, "Player-%d", id);
	genPlayer(name, id);
}

void cmdPlayerLeft(int id) {
	Player* pl = getPlayer();
	ListManager* players = getPlayerList();
	logger->inf("player killed Recieved : %d (I am %d)", id, pl->id);


	Node *tmp = NULL;
	while((tmp = listIterate(players, tmp)) != NULL) {
		Player *deadPlayer = tmp->value;

		if (deadPlayer->id == id) {
			killPlayer(deadPlayer);
		}
	}
}

void cmdBreakBlock(int id, BonusType type) {
	logger->dbg("Break Block By Id: %d", id);
	ListManager* blockList = getBlockList();

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


		generateBonus(bonuPos, type);
		deleteNode(blockList, block->id);
	}
}

void cmdBomb(int id) {
	logger->dbg("Bomb Placed By Id: %d", id);

	ListManager* players = getPlayerList();
	Node* playerNode = getNode(players, id);

	if(playerNode != NULL) {
		Player *player = playerNode->value;

		if(player != NULL && player->alive) {
			placeBomb(player);
		}
	}
}

void cmdMove(int id, int direction) {
	logger->dbg("Moving Player #%d: %d", id, direction);

	ListManager* players = getPlayerList();
	Node* playerNode = getNode(players, id);

	if(playerNode != NULL) {
		Player *player = playerNode->value;

		if(player != NULL && player->alive) {
			playerMove(player, direction);
		}
	}
}

void cmdStop(int id) {
	logger->dbg("Stopping Player #%d", id);

	ListManager* players = getPlayerList();
	Node* playerNode = getNode(players, id);

	if(playerNode != NULL) {
		Player *player = playerNode->value;

		if(player != NULL) {
			playerStop(player);
		}
	}
}

void cmdStatus(int status) {
	logger->inf("CHANGING STAUS %d", status);
	changeGameStatus(status);
}

void cmdBonus(int bId, int pId) {
	logger->dbg("Bonus #%d Taken By Player #%d", bId, pId);


	ListManager* bonusList = getBonusList();
	logger->dbg("### PRINTING BONUS LIST");

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

void clientCommand(char* msg) {
    //logger->err("Handeling Command: %s", msg);

  	char* resp[6];
  	memset(resp, 0, 6);
    int z;
  	for (z = 0; z < 6; ++z){
  		resp[z] = 0;
  	}

    explode(':', msg, 0, 0, resp);
    Player *pl = getPlayer();

    char* cmd = resp[0];



    Connexion* co = getConnexion();
    Node* n = getNodeByName(co->commands, cmd);

    //logger->err("Calling Command");
    //logger->err("Existing Commands: %d", co->commands->nodeCount);

    if(n != NULL) {
        Arg* arg = n->value;
<<<<<<< HEAD
	    //logger->err("CMD FOUND");
    	//logger->err("CMD: %s", cmd);
	    
	    if(arg->function == (void*)cmdRefresh) {
    		//logger->err("CMD REFRESH");
    		
    		//logger->war("t: %s", resp[0]);
    		//logger->war("t: %s", resp[1]);
    		//logger->war("t: %s", resp[2]);
    		//logger->war("t: %s", resp[3]);
    		//logger->war("t: %s", resp[4]);
    		
    		//logger->err("CMD REFRESH 2");
=======
	    logger->err("CMD FOUND");
    	logger->err("CMD: %s", cmd);

	    if(arg->function == (void*)cmdRefresh) {
    		logger->err("CMD REFRESH");

    		logger->war("t: %s", resp[0]);
    		logger->war("t: %s", resp[1]);
    		logger->war("t: %s", resp[2]);

    		logger->err("CMD REFRESH 2");
>>>>>>> sl/timer
        	arg->function(resp);
	    }
	    else if(resp[2] != 0) {
	    	//logger->err("Double Values");
    		int val = str2int(resp[1]);
	    	int val2 = str2int(resp[2]);
        	arg->function(val, val2);
	    }
	    else{
	    	//logger->err("Single Values");
    		int val = str2int(resp[1]);
        	arg->function(val);
	    }
    }
    else{
        logger->err("Fail To Find Command: %s", cmd);
    }

    //logger->err("Free Respnonse");
    for (z = 0; resp[z] != 0; ++z){
    	free(resp[z]);
    	resp[z] = 0;
    }
    
    //logger->err("### HANDLE DONE");
}

void initClientCommands(Connexion* co) {
	enableLogger(DBG_CLIENT);

	logger->inf("==== INIT CLIENT COMMANDS ====");
	co->commands = initListMgr();

	/*  NEW PLAYER  */
	Arg* arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdNewPlayer;

	logger->dbg("-- New Player");
	Node * n = addNodeV(co->commands, "newPlayer", arg, 1);



	/*  PLAYER DISCONNECT  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdPlayerLeft;

	logger->dbg("-- Player Left");
	n = addNodeV(co->commands, "playerLeft", arg, 1);



	/*  PLAYER KILLED  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdPlayerLeft;

	logger->dbg("-- Player Killed");
	n = addNodeV(co->commands, "playerkilled", arg, 1);



	/*  Block Break  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdBreakBlock;

	logger->dbg("-- Block Break");
	n = addNodeV(co->commands, "breackblock", arg, 1);



	/*  BOMB  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdBomb;

	logger->dbg("-- Place Bomb");
	n = addNodeV(co->commands, "bombPlaced", arg, 1);



	/*  MOVE  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdMove;

	logger->dbg("-- Player Move");
	n = addNodeV(co->commands, "move", arg, 1);



	/*  STOP  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdStop;

	logger->dbg("-- Player Stop");
	n = addNodeV(co->commands, "stop", arg, 1);



	/*  STATUS  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdStatus;

	logger->dbg("-- Change Status");
	n = addNodeV(co->commands, "status", arg, 1);



	/*  BONUS  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdBonus;

	logger->dbg("-- Bonus Taken");
	n = addNodeV(co->commands, "bonus", arg, 1);

	/*  REFRESH  */
	arg = malloc(sizeof(Arg));
	arg->function = (void*) cmdRefresh;

	logger->dbg("-- Refresh");
	n = addNodeV(co->commands, "refresh", arg, 1);
}

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
	initClientCommands(&connexion);

	logger->dbg("-- Connection SUCCESS");
	return &connexion;
}

Connexion* getConnexion() {
	return initConnexion(0);
}

void getMessage(char* msg) {
	memset(msg, 0, MSG_SIZE);
	Connexion* co = getConnexion();


	int i = recv(co->fd, msg, 1, 0);

	i = recv(co->fd, msg, (int)msg[0] +1, 0);
	logger->dbg("Received Msg: %s", msg);

	if(i < 0) {
		logger->err("Faild To Receive Message !!!");
	}
}

short findHost() {
	enableLogger(DBG_CLIENT);
	logger->inf("==== Findding Host ====");

	Connexion* co = initConnexion(1);

	//logger->war("Find: Un-Lock");
	unlock(DBG_STATE);

	if(co == NULL || !co->init) {
		return 0;
	}

	char msg[MSG_SIZE];
	getMessage(msg);
	logger->dbg("Response: %s");

	char* resp[3];
	explode(':', msg, 0, 0, resp);

	int id = char2int(resp[1][0]);
	logger->dbg("id : %d", id);

	free(resp[0]);
	free(resp[1]);

	if(id <= 0) {
		logger->err("Connexion Refuse !!!");
		return 0;
	}

	if(getServer() == NULL) {
		//logger->war("Find: Ask-Lock");
		lock(DBG_STATE);

		clearObjects();

		char name[12];
		Player* p = NULL;
		for(int i = 1; i <= id; i++) {
			memset(name, 0, 12);
			snprintf(name, 12, "player-%d", i);
			p = genPlayer(name, i);
		}

		initPlayer(p);
	}

	logger->inf("Connexion Accepted");

	return 1;
}


void* clientProcess() {
	logger->err("============== LAUNCH CLIENT =============");

	enableLogger(DBG_CLIENT);
	logger->inf("==== INIT CLIENT THREAD ====");


	Game* game = getGame();
	char msg[MSG_SIZE];

	fd_set readfds;
	struct timeval tv = {0, 20};
	Connexion* co = getConnexion();

	//logger->war("Client: Ask-Lock");
  	lock(DBG_CLIENT);
	//logger->war("Client: Lock");


	int z;
  	char* resp[5];
  	memset(resp, 0, 5);
	while(game->status < GAME_QUIT) {
		//logger->war("Client: Un-Lock");
	    unlock(DBG_CLIENT);
		enableLogger(DBG_CLIENT);

		for (z = 0; resp[z] != 0; ++z){
			free(resp[z]);
			resp[z] = 0;
		}
  		memset(resp, 0, 5);

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

		    logger->dbg("No Activity");

			//logger->dbg("Client: Ask-Lock");
	    	lock(DBG_CLIENT);
			//logger->dbg("Client: Lock");
		    continue;
		}

		enableLogger(DBG_CLIENT);
		logger->dbg("-- Checking fd");
		if (!FD_ISSET(co->fd, &readfds)) {
			enableLogger(DBG_CLIENT);

			logger->dbg("-- read fd is empty skipping");

			//logger->dbg("Client: Ask-Lock");
	    	lock(DBG_CLIENT);
			//logger->dbg("Client: Lock");

		    continue;
		}

		enableLogger(DBG_CLIENT);
		logger->dbg("-- Getting Message");
	  	getMessage(msg);
		logger->dbg("-- Got Message %s\nMsg Length:%d", msg, strlen(msg));

	    if(strlen(msg) == 0) {
			enableLogger(DBG_CLIENT);
			logger->dbg("-- msg is empty continue...");

			//logger->dbg("Client: Ask-Lock");
	    	lock(DBG_CLIENT);
			//logger->dbg("Client: Lock");
	    	continue;
	    }

		enableLogger(DBG_CLIENT);
		logger->dbg("-- Msg: %s", msg);

		//explode(':', msg, 0, 0, resp);
		//Player *pl = getPlayer();
		//logger->dbg("%d", pl->id);

		//logger->dbg("Client: Ask-Lock");
	    lock(DBG_CLIENT);
		//logger->dbg("Client: Lock");
		
		clientCommand(msg);
		logger->dbg("== CLIENT SLEEP ==");
	}

	/*for (z = 0; resp[z] != 0; ++z){
		free(resp[z]);
		resp[z] = 0;
	}
	memset(resp, 0, 5);*/

	close(co->fd);

	//logger->war("Client: Un-Lock");
    unlock(DBG_CLIENT);
    pthread_exit(NULL);
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

	logger->dbg("#### Sending Comand %s", msg);
	send(co->fd, msg, MSG_SIZE, 0);
}
