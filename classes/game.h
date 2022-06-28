/*****************************************************************//**
 * \file   game.h
 * \brief  Handles initialisation of SDL2, creates a window, renderer
 * and handles many other game specific operations such as delta
 * fps calculation, texture handling and event handling.
 * 
 * \author Jason Hewitt <bowedyapper@live.co.uk>
 * \date   June 2022
 *********************************************************************/

#include <assert.h>
#include <vector>
class Game {
private:
	
	const char* windowTitle;

	/**
	 * \brief 
	 * 
	 * \return true if init was successful
	 * \return false if init threw and error
	 */
	bool init();
	bool createWindow(int width, int height);
	bool createRenderer();

public:
	bool isRunning = true;
	bool chatOpen = false;
	double delta = 0;

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
	auto getSystemTime();

};

/**
 * \brief Initalises all the nessecary SDL libraries.
 * \returns True if successful.
 */
bool Game::init() {
#if EE_CURRENT_PLATFORM == EE_PLATFORM_WINDOWS
	
	printf("Environment is windows, setting audio driver to Directsound\n");
	SDL_setenv("SDL_AUDIODRIVER", "directsound", true);
#endif
	bool ttfInit = TTF_Init();

	// load support for the JPG and PNG image formats
	int flags = IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return false;
	}

	printf("Initalising SDL..");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0 && ttfInit != 0)
	{
		printf("error");
		throw("Error initializing SDL: %s\n", SDL_GetError());
		return false;
	}
	printf("Success\n");

	isRunning = true;
	return true;
}

/**
 * \brief Creates a window with a specified width and height.
 * \param width An integer specifiying the width of the window.
 * \param height An integer specifiying the height of the window.
 * \returns True if successful.
 */
bool Game::createWindow(int width, int height) {
	windowWidth = width;
	windowHeight = height;
	
	printf("Creating %d x %d window..", width, height);
	//SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // May need to be enabled for UNIX systems
	window = SDL_CreateWindow(windowTitle,
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		SDL_WINDOWPOS_CENTERED_DISPLAY(1),
		windowWidth, windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	if (!window)
	{
		printf("error");
		throw("Error creating window: %s\n", SDL_GetError());
		return false;
	}
	std::cout << "Success" << std::endl;
	return true;
}

bool Game::createRenderer() {
	printf("Initialising renderer...");
	/* Create a renderer */
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
	if (!renderer)
	{
		throw("Error creating renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		return false;
	}
	std::cout << "Success" << std::endl;
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
	std::cout << "Quitting game" << std::endl;
	SDL_Quit();
	isRunning = false;
}

void Game::eventHandler(SDL_Event &event) {
	if (event.type == SDL_KEYDOWN) {
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
			printf("Game was closed");
			isRunning = false;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				std::cout << "Resizing window - " << event.window.data1 << event.window.data2 << std::endl;
				resizeWindow(event.window.data1, event.window.data2);
				break;
			}
			
			//switch (event.key.keysym.scancode) {
			//	
			//	case SDL_SCANCODE_F:
			//		/*if (currentFpsSelection == 9) {
			//			currentFpsSelection = 0;
			//		}
			//		else {
			//			currentFpsSelection += 1;
			//		}
			//		FPS = (float)fpsArray[currentFpsSelection];*/
			//		//std::cout << "FPS changed to " << FPS << std::endl;
			//		break;

			//	case SDL_SCANCODE_C:
			//		//player.changeColour();
			//		break;

			//	case SDL_SCANCODE_R:
			//		//player.x = 0;
			//		//player.y = 0;
			//		//printf("Position reset");
			//		break;

			//	case SDL_SCANCODE_RETURN:
			//		chatOpen = true;
			//		break;

			//	case SDL_SCANCODE_ESCAPE:
			//		
			//		break;
			//			
			
		//}
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
	return SDL_RenderCopyExF(renderer, texture, clip, &textureRect, NULL, NULL, SDL_FLIP_NONE);
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

auto Game::getSystemTime(){
	auto cur_time = std::chrono::system_clock::now();
	std::cout << "TIME: " << std::chrono::system_clock::to_time_t(cur_time) << std::endl;
	auto unixtime = cur_time.time_since_epoch();
	auto unixtime_in_s = std::chrono::duration_cast<std::chrono::seconds>(unixtime);
	return unixtime_in_s.count();
}
