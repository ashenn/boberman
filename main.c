#include "common.h"

Log* logger;

Game* getGame() {
	static Game* game = NULL;
	
	if (game != NULL) {
		return game;
	}

	logger->inf("==== INIT GAME ====");	
	game = malloc(sizeof(game));
	game->status = GAME_MENU;

	return game;
}

void* closeApp() {
	logger->inf("==== Closing App ====");	
	Game* game = getGame();

	// Cleaning Objects
	Node* obj = NULL;
	ListManager* objects = getObjectList();

	logger->dbg("-- Cleaning Objects");
	while((obj = listIterate(objects, obj)) != NULL) {
		logger->dbg("-- Obj: %s", obj->name);
		deleteObject(obj->value);
	}

	logger->dbg("-- Quit SDL");
	game->status = GAME_QUIT;
	SDL_Quit();
	
	logger->dbg("==== Killing Game Proccess ====");
}

void quitGame() {
	logger->inf("==== Quitting Game ====");	
	Game* game = getGame();
	game->status = GAME_QUIT;
}

void* sayHello() {
	logger->err("Hello !");
}

void* sayGoodbye() {
	logger->err("Goodbye !");
}

Button** getMenu() {
	logger->inf("==== Getting Menu ====");	
	
	static Button btnJoin = {
		.z = 2,
		.name = "Find",
		.text = "Find",
		.fontSize = FONT_LG,
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) sayHello,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};
	

	// Define Button Size
	btnJoin.pos.w = LG_BTN_W;
	btnJoin.pos.h = LG_BTN_H;

	// Define Position
	btnJoin.pos.x = SCREEN_W + 5;
	btnJoin.pos.y = percent(45, SCREEN_H);
	
	// Define Animation Position
	btnJoin.anim = malloc(sizeof(AnimParam));
	btnJoin.anim->time = 1.5f;
	btnJoin.anim->delay = 0;
	btnJoin.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnJoin.anim->pos.y = percent(45, SCREEN_H);
	
	btnJoin.anim->pos.w = 0;
	btnJoin.anim->pos.h = 0;
	
	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnJoin.name,
		btnJoin.pos.x,
		btnJoin.pos.y,
		btnJoin.pos.w,
		btnJoin.pos.h,
		btnJoin.anim->pos.x,
		btnJoin.anim->pos.y
	);

	static Button btnHost = {
		.z = 2,
		.name = "Host",
		.text = "Host",
		.fontSize = FONT_LG,
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) sayGoodbye,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};

	// Define Button Size
	btnHost.pos.w = LG_BTN_W;
	btnHost.pos.h = LG_BTN_H;

	// Define Position
	btnHost.pos.x = SCREEN_W + 5;
	btnHost.pos.y = btnJoin.anim->pos.y + (LG_BTN_H  * 1.25);


	// Define Animation Position
	btnHost.anim = malloc(sizeof(AnimParam));
	btnHost.anim->time = 1.5f;
	btnHost.anim->delay = 0.2f;
	btnHost.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnHost.anim->pos.y = btnJoin.anim->pos.y + (LG_BTN_H  * 1.25);
	
	btnHost.anim->pos.w = 0;
	btnHost.anim->pos.h = 0;

	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnHost.name,
		btnHost.pos.x,
		btnHost.pos.y,
		btnHost.pos.w,
		btnHost.pos.h,
		btnHost.anim->pos.x,
		btnHost.anim->pos.y
	);

	static Button btnQuit = {
		.z = 2,
		.name = "Quit",
		.text = "Quit",
		.fontSize = FONT_LG,
		.imgPath = "lg-button-red",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) quitGame,
		.hover = (void*) buttonHover,
		.pos = { 0, 0, 0, 0 },
		.hasAnim = 1
	};

	// Define Button Size
	btnQuit.pos.w = LG_BTN_W;
	btnQuit.pos.h = LG_BTN_H;

	// Define Position
	btnQuit.pos.x = SCREEN_W + 5;
	btnQuit.pos.y = btnHost.anim->pos.y + (LG_BTN_H  * 1.25);



	// Define Animation Position
	btnQuit.anim = malloc(sizeof(AnimParam));
	btnQuit.anim->time = 1.5f;
	btnQuit.anim->delay = 0.4f;
	btnQuit.anim->pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnQuit.anim->pos.y = btnHost.anim->pos.y + (LG_BTN_H  * 1.25);

	btnQuit.anim->pos.w = 0;
	btnQuit.anim->pos.h = 0;

	logger->dbg(
		"-- Button: %s\n--pos: x: %d | y: %d | w: %d | h: %d\n--anim: x: %d | y: %d | w: %d | h: %d\n",
		btnQuit.name,
		btnQuit.pos.x,
		btnQuit.pos.y,
		btnQuit.pos.w,
		btnQuit.pos.h,
		btnQuit.anim->pos.x,
		btnQuit.anim->pos.y
	);


	static Button* btns[] = {
		&btnJoin,
		&btnHost,
		&btnQuit,
		NULL
	};

	return btns;
}

void handleEvents() {
	SDL_Event event;
	Game* game = NULL;
	static Object** curHovered = NULL;
	
	static int lastX = 0;
	static int lastY = 0;
	
	Object* clicked = NULL;
	Object* hovered = NULL;

	if (game == NULL) {
		game = getGame();
	}

	logger->inf("==== EVENTS ====");
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				logger->dbg("-- EVT QUIT");
				game->status = GAME_QUIT;
				break;

			case SDL_KEYDOWN:
				logger->dbg("-- EVT KEY DOWN");
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
					case SDLK_q:
						game->status = GAME_QUIT;
						break;
					
					default:
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				logger->dbg("-- EVT KEY BUTTON");

				clicked = getClicked(event.button.x, event.button.y, 0);
				if (clicked != NULL && clicked->enabled && clicked->click != NULL) {
					clicked->click(clicked);
				}
            	break;

			case SDL_MOUSEMOTION:
				logger->dbg("-- EVT KEY MOVE");

				if (lastX == event.button.x && lastY == event.button.y) {
					break;
				}

				lastX = event.button.x;
				lastY = event.button.y;

				hovered = getClicked(event.button.x, event.button.y, 1);
				curHovered = getHovered();

				if (hovered == *curHovered) {
					break;
				}

				if (*curHovered != NULL && hovered != *curHovered){
					buttonUnHover(*curHovered);
				}
			
				*curHovered = hovered;

				if (hovered != NULL && hovered->enabled && hovered->hover != NULL) {

					logger->inf("-- Calling hover For: %s", hovered->name);
					hovered->hover(hovered);
				}

            	break;
		}
	}

	logger->dbg("==== DONE ====");
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

	//logger->inf("-- Init: Get Title Screen");
	char titlePath[] ="asset/title.png";


	//logger->inf("-- Init: Setting Background");
	SDL_Surface* bg = IMG_Load(titlePath);

	//logger->inf("-- Init: Adding Background");
	addSimpleObject("Background", bg, NULL, 1);

	//logger->inf("-- Init: Button");
	int i;
	Button** btns = getMenu();
	for (i = 0; btns[i] != NULL; ++i){
		//logger->dbg("Adding Button: %s", btns[i]->text);
		generateButton(btns[i]);
	}
	
	SDL_Event event;
	Object* obj = NULL;
	Object* hovered = NULL;
	Object** curHovered = NULL;

	i = 0;
	int delay = 0;
	int lastX = 0;
	int lastY = 0;

	int t_deb = 0;
	int t_fin = 0;
	int t_boucle = 1000 / FPS;

	Game* game = getGame();
	while (game->status == GAME_MENU){
		logger->inf("==== Tick #%d ====", i++);
		int now = SDL_GetTicks(); 
		handleEvents();

		render();
    	animate();

		t_fin = SDL_GetTicks();
        if (t_fin - t_deb < t_boucle) {
		
			t_fin = SDL_GetTicks();

			delay = t_boucle - (t_fin - t_deb);
			if (delay > 0){
        		SDL_Delay(t_boucle - (t_fin - t_deb));
			}
        };
        t_deb = SDL_GetTicks();

		logger->inf("==== Tick END ====");

//		break;
	}

	
	closeApp();
	logger->err("Good Bye ! :)");

	return 0;
}