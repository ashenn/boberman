#include "common.h"

Log* logger;

Game* getGame() {
	static Game* game = NULL;
	
	if (game != NULL) {
		return game;
	}
	
	game = malloc(sizeof(game));
	game->status = GAME_MENU;

	return game;
}

void* closeApp() {
	Game* game = getGame();
	game->status = GAME_QUIT;
	SDL_Quit();
}

void quitGame() {
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
	
	static Button btnJoin = {
		.z = 2,
		.name = "Find",
		.text = "Find",
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) sayHello,
		.hover = (void*) buttonHover,
		.hasAnim = 1,
		.time = 0.4f,
		.delay = 0,
		.pos = { 
			SCREEN_W + 5, 
			0, 0, 0 
		},
		.anim = {
			.time = 0.4f,
			.delay = 0
		}
	};

	// btnJoin.pos.x = -LG_BTN_W;
	btnJoin.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnJoin.pos.x = SCREEN_W + 5;
	
	btnJoin.anim.pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);

	btnJoin.y = percent(45, SCREEN_H);
	//btnJoin.pos.y = 270;
	btnJoin.pos.y = percent(45, SCREEN_H);

	logger->war("### BTN TIME: %f | %f", btnJoin.x, btnJoin.pos.x);
	logger->war("### BTN TIME: %f | %f", percent(50, SCREEN_W), (LG_BTN_W / 2));
	
	btnJoin.pos.w = LG_BTN_W;
	btnJoin.pos.h = LG_BTN_H;


	static Button btnHost = {
		.z = 2,
		.name = "Host",
		.text = "Host",
		.pos = { 0, 0, 0, 0},
		.imgPath = "lg-button-green",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) sayGoodbye,
		.hover = (void*) buttonHover,
		.hasAnim = 1,
		.time = 0.4f,
		.delay = 0.1f
	};

	btnHost.pos.x = SCREEN_W + 5;
	//btnHost.pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnHost.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);

	btnHost.pos.y = btnJoin.y + (LG_BTN_H  * 1.25);
	btnHost.y = btnJoin.y + (LG_BTN_H  * 1.25);

	btnHost.pos.w = LG_BTN_W;
	btnHost.pos.h = LG_BTN_H;

	static Button btnQuit = {
		.z = 2,
		.name = "Quit",
		.text = "Quit",
		.pos = { 0, 0, 0, 0},
		.imgPath = "lg-button-red",
		.imgHoverPath = NULL,
		.imgObj = NULL,
		.txtObj = NULL,
		.click = (void*) quitGame,
		.hover = (void*) buttonHover,
		.hasAnim = 1,
		.time = 0.4f,
		.delay = 0.2f
	};

	btnQuit.pos.x = SCREEN_W + 5;
	//btnQuit.pos.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);
	btnQuit.x = percent(50, SCREEN_W) - (LG_BTN_W / 2);

	btnQuit.pos.y = btnHost.y + (LG_BTN_H  * 1.25);
	btnQuit.y = btnHost.y + (LG_BTN_H  * 1.25);

	btnQuit.pos.w = LG_BTN_W;
	btnQuit.pos.h = LG_BTN_H;
	//logger->err("%d | %d | %d | %d", btnQuit.pos.x, btnQuit.pos.y, btnQuit.pos.x + btnQuit.pos.w, btnQuit.pos.y + btnQuit.pos.h);

	static Button* btns[] = {
		&btnJoin,
		&btnHost,
		&btnQuit,
		NULL
	};

	return btns;
}

void handleEvents() {
	static SDL_Event event;
	static Game* game = NULL;
	static Object* obj = NULL;
	static Object* hovered = NULL;
	static Object** curHovered = NULL;

	static int lastX = 0;
	static int lastY = 0;

	if (game == NULL) {
		 game= getGame();
	}

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				game->status = GAME_QUIT;
				break;

			case SDL_KEYDOWN:
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
				obj = getClicked(event.button.x, event.button.y, 0);
				if (obj != NULL && obj->enabled && obj->click != NULL) {
					obj->click(obj);
				}
            	break;

			case SDL_MOUSEMOTION:
				if (lastX == event.button.x || lastY == event.button.x) {
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
					buttonUnHover();
				}
			
				*curHovered = hovered;

				if (hovered != NULL && hovered->enabled && hovered->hover != NULL) {

					logger->inf("Calling hover For: %s", hovered->name);
					hovered->hover(hovered);
				}

            	break;
		}
	}
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
		// logger->err("Frame: #%d", i++);
		int now = SDL_GetTicks(); 
		handleEvents();

    	animate();
		render();

		//logger->war("-- Tick Calculation -- ");
		t_fin = SDL_GetTicks();
        if (t_fin - t_deb < t_boucle) {
		
			t_fin = SDL_GetTicks();

			delay = t_boucle - (t_fin - t_deb);
			if (delay > 0){
				//logger->war("Tick Wait: %d", t_boucle - (t_fin - t_deb));
        		SDL_Delay(t_boucle - (t_fin - t_deb));
			}
        };
        t_deb = SDL_GetTicks();

        if (i++ == 5){
			//deleteObject(btns[0]->imgObj);
			//break;
        }
		//logger->war("-- Tick END -- ");
	}

	SDL_FreeSurface(bg);
	closeApp();

	return 0;
}