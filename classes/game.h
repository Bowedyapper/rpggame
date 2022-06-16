/*****************************************************************//**
 * \file   game.h
 * \brief  Handles initialisation of SDL2, creates a window, renderer
 * and handles many other game specific operations such as delta
 * fps calculation, texture handling and event handling.
 * 
 * \author Jason Hewitt <bowedyapper@live.co.uk>
 * \date   June 2022
 *********************************************************************/
#include "texture.h"
class Game {
private:
	int windowWidth;
	int windowHeight;
	char* windowTitle = "";
	bool init();
	bool createWindow(int width, int height);
	bool createRenderer();
public:
	bool isRunning;

	SDL_Window* window;
	SDL_Renderer* renderer;
	TextureCache mainTextures;
	
	double delta = 0;
	Uint64 timeNow = SDL_GetPerformanceCounter();;
	Uint64 previousTime = 0;
	Game(int windowWidth, int windowHeight, char* title ){
		init();
		windowTitle = title;
		createWindow(windowWidth, windowHeight);
		createRenderer();
		
		//TODO: Work out why this doesn't work as a constructor
		mainTextures.assignRenderer(renderer);
	}
	
	void setTitle(char* title);
	void resizeWindow(int w, int h);
	void clearScreen();
	void renderPresent();
	void quit();
	void eventHandler(SDL_Event &event);
	double calculateDeltaTime();
	double calcFps();

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

	if (!ttfInit) return false;

	// load support for the JPG and PNG image formats
	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	int initted = IMG_Init(flags);
	if ((initted & flags) != flags) {
		printf("IMG_Init: Failed to init required jpg and png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return false;
	}

	printf("Initalising SDL..");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0 && ttfInit)
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
	window = SDL_CreateWindow(windowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);
	if (!window)
	{
		printf("error");
		throw("Error creating window: %s\n", SDL_GetError());
		return false;
	}
	printf("Success\n");
	return true;
}

bool Game::createRenderer() {
	printf("Initialising renderer...");
	/* Create a renderer */
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	renderer = SDL_CreateRenderer(window, -1, render_flags);
	if (!renderer)
	{
		throw("Error creating renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		return false;
	}
	printf("Success\n");
	SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	return true;
}

void Game::setTitle(char* title) {
	SDL_SetWindowTitle(window, title);
}

void Game::resizeWindow(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	SDL_SetWindowSize(window, w, h);

}

void Game::clearScreen() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

void Game::renderPresent() {
	SDL_RenderPresent(renderer);
}
void Game::quit() {
	std::cout << "Quitting game" << std::endl;
	SDL_Quit();
	isRunning = false;
}

void Game::eventHandler(SDL_Event &event) {
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
		break;

	case SDL_KEYDOWN:
		switch (event.key.keysym.scancode) {
		case SDL_SCANCODE_F:
			/*if (currentFpsSelection == 9) {
				currentFpsSelection = 0;
			}
			else {
				currentFpsSelection += 1;
			}
			FPS = (float)fpsArray[currentFpsSelection];*/
			//std::cout << "FPS changed to " << FPS << std::endl;
			break;

		case SDL_SCANCODE_C:
			//player.changeColour();
			break;

		case SDL_SCANCODE_R:
			//player.x = 0;
			//player.y = 0;
			printf("Position reset");
			break;
		}
		break;
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

