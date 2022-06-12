class Game {
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	int HEIGHT, WIDTH;
	Game() {}
	Game(int height, int width) {
		HEIGHT = height;
		WIDTH = width;
		#if EE_CURRENT_PLATFORM == EE_PLATFORM_WINDOWS
		printf("Environment is windows, setting audio driver to Directsound\n");
			SDL_setenv("SDL_AUDIODRIVER", "directsound", true);
		#endif

			printf("Initalising SDL..");
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			printf("error");
			throw("Error initializing SDL: %s\n", SDL_GetError());
		}
		printf("Success\n");
			printf("Creating window..");
		 window = SDL_CreateWindow("",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
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
		WIDTH = w;
		HEIGHT = h;
		SDL_SetWindowSize(window, w, h);

	}
private:
};