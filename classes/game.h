/*****************************************************************//**
 * @file   game.h
 * @brief  Handles initialisation of SDL2, creates a window, renderer
 * and handles many other game specific operations such as delta
 * fps calculation, texture handling and event handling.
 * 
 * @author Jason Hewitt <bowedyapper@live.co.uk>
 * @date   June 2022
 *********************************************************************/
class Game {
private:
	
	const char* windowTitle;

	/**
	 * @brief 
	 * 
	 * @return true if init was successful
	 * @return false if init threw and error
	 */
	bool init();
	bool createWindow(int width, int height);
	bool createRenderer();

public:
	bool isRunning = true;
	bool chatOpen = false;
	bool detachedCamera = false;
	double delta = 0;


int texX = 0;
	int texY = 0;

	
	int mousePosX;
	int mousePosY;
	int windowWidth;
	int windowHeight;
	Uint64 timeNow = SDL_GetPerformanceCounter();;
	Uint64 previousTime = 0;

	SDL_Event event;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TextureCache mainTextures;
	
	Game(int windowWidth, int windowHeight, const char* title){
		init();
		windowTitle = title;
		createWindow(windowWidth, windowHeight);
		createRenderer();
		
		//TODO: Work out why this doesn't work as a constructor
		mainTextures.assignRenderer(renderer);
	}
	
	void setTitle(const char* title);
	void resizeWindow(int w, int h);
	void clearScreen(int r = 0, int g = 0, int b = 0, int alpha = 255);
	void render();
	void quit();
	void eventHandler(SDL_Event& event);
	double calculateDeltaTime();
	double calcFps();
	void pollEvents();
	int clipRenderTexture(SDL_Texture* texture, float w, float h, float x, float y, SDL_Rect* clip);

	auto renderText(const char* fontFile, float fontSize, const char* text, SDL_Color colour);
	int displayText(const char* fontFile, float fontSize, std::string text, float x, float y, SDL_Color colour);
};

/**
 * @brief Initialises all the necessary SDL libraries.
 * @returns True if successful.
 */
bool Game::init() {
#ifdef _WIN32
	utils::debugLog("info", "Environment is windows, setting audio driver to Directsound");
	SDL_setenv("SDL_AUDIODRIVER", "directsound", true);
#endif

	bool ttfInit = TTF_Init();

	// load support for the JPG and PNG image formats
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		utils::debugLog("error", "Failed to init required png support: " + (std::string)IMG_GetError());
		return false;
	}

	utils::debugLog("info", "Initialising SDL..");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0 && ttfInit != 0)
	{
		utils::debugLog("error" , "Error initializing SDL:" + (std::string)SDL_GetError());
		return false;
	}
	utils::debugLog("info", "Initialising SDL successful");

	isRunning = true;
	return true;
}

/**
 * @brief Creates a window with a specified width and height.
 * @param width An integer specifying the width of the window.
 * @param height An integer specifying the height of the window.
 * @returns True if successful.
 */
bool Game::createWindow(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	utils::debugLog("info", "Attempting to create window");
	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // May need to be enabled for UNIX systems
	window = SDL_CreateWindow(windowTitle,
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window)
	{
		utils::debugLog("error", "Error creating window:" + (std::string)SDL_GetError());
		return false;
	}
	utils::debugLog("info", std::to_string(windowWidth) + " x " + std::to_string(windowHeight) + " window created.");
	return true;
}

bool Game::createRenderer() {
	utils::debugLog("info", "Initialising renderer");
	/* Create a renderer */
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
	if (!renderer)
	{
		utils::debugLog("error", "Error creating renderer: " + (std::string)SDL_GetError());
		SDL_DestroyWindow(window);
		return false;
	}
	utils::debugLog("info", "Renderer created");
	SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	return true;
}

void Game::setTitle(const char* title) {
	SDL_SetWindowTitle(window, title);
}

void Game::resizeWindow(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	SDL_SetWindowSize(window, w, h);

}

void Game::clearScreen(int r, int g, int b, int alpha) {
	SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
	SDL_RenderClear(renderer);
}

void Game::render() {
	SDL_RenderPresent(renderer);
}
void Game::quit() {
	utils::debugLog("info", "Exiting game");
	SDL_Quit();
	isRunning = false;
}

void Game::eventHandler(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_2:
					detachedCamera = !detachedCamera;
					utils::debugLog("info", (detachedCamera) ? "Detached camera" : "Reattached camera");
					break;

				case SDL_SCANCODE_RETURN:
					chatOpen = true;
					SDL_StartTextInput();
					break;

				case SDL_SCANCODE_KP_PLUS:
					texX +=1;
					break;

				case SDL_SCANCODE_ESCAPE:
					chatOpen = false;
					break;
			}
		if (event.key.keysym.scancode){
			//keys::handleKeyDown(event.key.keysym.scancode);
		}
	}
	if (event.type == SDL_KEYUP)
	{
		if(event.key.keysym.scancode) {
			//keys::handleKeyUp(event.key.keysym.scancode);
		}
	}
	switch (event.type) {
		case SDL_QUIT:
			quit();
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Resizing window - " << event.window.data1 << event.window.data2 << std::endl;
				resizeWindow(event.window.data1, event.window.data2);
				break;
			}
	}
}

double Game::calculateDeltaTime() {
	previousTime = timeNow;
	timeNow = SDL_GetPerformanceCounter();
	delta = (double)(timeNow - previousTime) / (double)SDL_GetPerformanceFrequency() * 1000.0f;
	return delta;
}

double Game::calcFps() {
		double fps = (double)1000.0f / (double)delta;
		return fps;
}

void Game::pollEvents() {
	SDL_GetMouseState(&mousePosX, &mousePosY);
	if (SDL_PollEvent(&event)) {
		eventHandler(event);
	}
}

int Game::clipRenderTexture(SDL_Texture* texture, float w, float h, float x, float y, SDL_Rect* clip) {
	SDL_FRect textureRect;
	textureRect.x = x;
	textureRect.y = y;

	if (clip != NULL)
	{
		textureRect.w = (float)clip->w;
		textureRect.h = (float)clip->h;
	}
	else {
		textureRect.w = w;
		textureRect.h = h;
	}
	return SDL_RenderCopyExF(renderer, texture, clip, &textureRect, 0.0, NULL, SDL_FLIP_NONE);
}

auto Game::renderText(const char* fontFile, float fontSize, const char* text, SDL_Color colour) {
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;
	auto font = TTF_OpenFont(fontFile, (int)fontSize);
	textSurface = TTF_RenderText_Blended_Wrapped(font, text, colour, 0);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	TTF_CloseFont(font);
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	TextureObject returnClass(textTexture, textWidth, textHeight);
	SDL_FreeSurface(textSurface);

	return returnClass;
}


int Game::displayText(const char* fontFile, float fontSize, std::string text, float x, float y, SDL_Color colour) {
	SDL_Texture* textTexture;
	SDL_FRect textRect;
	TextureObject texture = renderText(fontFile, fontSize, text.c_str(), colour);
	textTexture = texture.texture;
	if (x == -1 && y == -1) {
		textRect.x = (float)windowWidth / 2 - texture.width / 2;
		textRect.y = (float)windowHeight / 2 - texture.height / 2;;
	}
	else {
		textRect.x = x;
		textRect.y = y;
	}

	textRect.w = (float)texture.width;
	textRect.h = (float)texture.height;

	SDL_RenderCopyF(renderer, texture.texture, NULL, &textRect);
	SDL_DestroyTexture(texture.texture);

	return 0;

}


