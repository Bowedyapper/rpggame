class Game {
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	Game() {
		#if EE_CURRENT_PLATFORM == EE_PLATFORM_WINDOWS
		printf("Environment is windows, setting audio driver to Directsound\n");
			SDL_setenv("SDL_AUDIODRIVER", "directsound", true);
		#endif
			bool ttfInit = TTF_Init();


			// load support for the JPG and PNG image formats
			int flags = IMG_INIT_JPG | IMG_INIT_PNG;
			int initted = IMG_Init(flags);
			if ((initted & flags) != flags) {
				printf("IMG_Init: Failed to init required jpg and png support!\n");
				printf("IMG_Init: %s\n", IMG_GetError());
				// handle error
			}

			printf("Initalising SDL..");
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0 && ttfInit)
		{
			printf("error");
			throw("Error initializing SDL: %s\n", SDL_GetError());
		}
		printf("Success\n");
			printf("Creating window..");
		 window = SDL_CreateWindow(windowTitle,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
		if (!window)
		{
			printf("error");
			throw("Error creating window: %s\n", SDL_GetError());
			SDL_Quit();
		}
		printf("Success\n");
		printf("Initialising renderer...");
		/* Create a renderer */
		Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
		renderer = SDL_CreateRenderer(window, -1, render_flags);
		if (!renderer)
		{
			throw("Error creating renderer: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
		printf("Success\n");
	}

	void setTitle(char* title) {
		SDL_SetWindowTitle(window, title);
	}

	void resizeWindow(int w, int h) {
		windowWidth = w;
		windowHeight = h;
		SDL_SetWindowSize(window, w, h);

	}
private:
};