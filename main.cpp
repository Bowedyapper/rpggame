#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <future>
#include <string>
#include <future>
#include <ctime>
#include <map>
#include "globals.h"
#include "colours.cpp"
#include "texture.cpp"
#include "game.cpp"
#include "character.cpp"
#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

typedef std::pair<std::string, Texture> textureKeyPair;
typedef std::_Vector_iterator<std::_Vector_val<std::_Simple_types<Character>>> characterVectorIterator;

int delta;
int ptime;    // Time since last frame
int offset = 0;
int tickRate = 1000 / 60;
int currentFpsSelection = 0;
int fpsArray[10] = {
	10, 15, 20, 25, 30, 40, 50, 60, 70, 140
};

bool running = true;
bool connected = false;

std::time_t ms;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;
std::string currentUserSocketId;
sio::client client;
std::map<std::string, Texture> textures;
Game gameObject;
Character player("player");
SDL_Event event;
TTF_Font* font;

int deltaTime(int previous, int offset) {
	int Δt = (clock() - previous);  // Δt = Current time - time of last frame
	return Δt;
}

void clearScreen() {
	SDL_SetRenderDrawColor(gameObject.renderer, 0, 0, 0, 255);
	SDL_RenderClear(gameObject.renderer);
}

void onConnection(sio::event& evnt) {
	std::string user = evnt.get_message()->get_map()["user"]->get_string();
	printf("Client %s connected\n", user.c_str());
	currentUserSocketId = user;
	Character player(user);
	playerVector.push_back(player);
	connected = true;
}

auto renderText(char* fontFile, int fontSize, char* text, SDL_Color colour) {
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;
	font = TTF_OpenFont(fontFile, fontSize);
	textSurface = TTF_RenderText_Blended_Wrapped(font, text, colour, 0);
	textTexture = SDL_CreateTextureFromSurface(gameObject.renderer, textSurface);
	TTF_CloseFont(font);
	int textWidth = textSurface->w;
	int textHeight = textSurface->h;
	Texture returnClass(textTexture, textWidth, textHeight);
	SDL_FreeSurface(textSurface);

	return returnClass;
}

int displayText(char* fontFile, int fontSize, char* text, int x, int y, SDL_Color colour) {
	SDL_Texture* textTexture;
	SDL_Rect textRect;
	Texture texture = renderText(fontFile, fontSize, text, colour);
	textTexture = texture.texture;
	textRect.x = x;
	textRect.y = y;
	textRect.w = texture.width;
	textRect.h = texture.height;

	return SDL_RenderCopy(gameObject.renderer, textTexture, NULL, &textRect);
}

int displayTexture(SDL_Texture* texture, int w, int h, int x, int y) {
	SDL_Rect textureRect;
	textureRect.x = x;
	textureRect.y = y;
	textureRect.w = w;
	textureRect.h = h;
	return SDL_RenderCopy(gameObject.renderer, texture, NULL, &textureRect);
}

Texture getTexture(std::string textureName) {
	auto getTextureFromMap = textures.at(textureName);
	return getTextureFromMap;
}
void eventHandler(SDL_Event event) {
	switch (event.type) {
	case SDL_QUIT:
		printf("Game was closed");
		running = false;
		break;

	case SDL_WINDOWEVENT:
		switch (event.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			std::cout << "Resizing window - " << event.window.data1 << event.window.data2 << std::endl;
			gameObject.resizeWindow(event.window.data1, event.window.data2);
			break;
		}
		break;

	case SDL_KEYDOWN:
		switch (event.key.keysym.scancode) {
		case SDL_SCANCODE_F:
			if (currentFpsSelection == 9) {
				currentFpsSelection = 0;
			}
			else {
				currentFpsSelection += 1;
			}
			FPS = fpsArray[currentFpsSelection];
			std::cout << "FPS changed to " << FPS << std::endl;
			break;

		case SDL_SCANCODE_C:
			player.changeColour();
			break;

		case SDL_SCANCODE_R:
			player.x = 0;
			player.y = 0;
			printf("Position reset");
			break;
		}
		break;
	}
}

void movementHandler() {
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {

		player.move("left");
		sio::message::list move("l");
		move.push(sio::int_message::create(delta));
		move.push(sio::int_message::create(player.rect.x));
		move.push(sio::int_message::create(ms));
		client.socket()->emit("move", move);
	}

	if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {

		player.move("right");
		sio::message::list move("r");
		move.push(sio::int_message::create(delta));
		move.push(sio::int_message::create(player.rect.x));
		move.push(sio::int_message::create(ms));
		client.socket()->emit("move", move);

	}

	if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
		player.move("up");
		sio::message::list move("u");
		move.push(sio::int_message::create(delta));
		move.push(sio::int_message::create(player.rect.y));
		move.push(sio::int_message::create(ms));
		client.socket()->emit("move", move);

	}

	if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
		player.move("down");
		sio::message::list move("d");
		move.push(sio::int_message::create(delta));
		move.push(sio::int_message::create(player.rect.y));
		move.push(sio::int_message::create(ms));
		client.socket()->emit("move", move);
	}
}

void calcDeltaTime() {
	delta = deltaTime(ptime, offset);
	ptime = clock();
	offset = delta % (1000 / tickRate);
	ptime = clock();
	ms = std::time(nullptr);
}

void drawRemotePlayers() {
	for (int ii = 0; ii < playerVector.size(); ii++) {
		if (playerVector[ii].socketid != currentUserSocketId) {
			playerVector[ii].draw();
		}
	}
}

void gameChunkUpdate(sio::event& evnt) {
	size_t playerChunkSize = evnt.get_message()->get_vector().size();
	playerChunk = evnt.get_message()->get_vector();

	for (int ii = 0; ii < playerChunkSize; ii++) {
		std::string user = evnt.get_message()->get_vector()[ii]->get_map()["socketid"]->get_string();
		double x = evnt.get_message()->get_vector()[ii]->get_map()["x"]->get_double();
		double y = evnt.get_message()->get_vector()[ii]->get_map()["y"]->get_double();

		if (user != currentUserSocketId) {
			characterVectorIterator findUser = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
			if (findUser != playerVector.end()) {
				findUser->pos(static_cast<int>(x), static_cast<int>(y));
			}
			else {
				Character newPlayer(user);
				playerVector.push_back(newPlayer);
			}
		}
	}
}

void handleUserDisconnect(sio::event& evnt) {
	std::string user = evnt.get_message()->get_string();
	auto it = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
	if (it != playerVector.end()) {
		auto index = std::distance(playerVector.begin(), it);
		playerVector.erase(playerVector.begin() + index);
	}
}

void mainMenu() {

	Texture titleImg = getTexture("title");
	Texture playBtn = getTexture("Play");
	Texture optionsBtn = getTexture("Options");

	displayTexture(titleImg.texture, titleImg.width, titleImg.height, (windowWidth / 2) - titleImg.width / 2, 10);
	displayTexture(playBtn.texture, playBtn.width, playBtn.height, (windowWidth / 2) - playBtn.width / 2, 350);
	displayTexture(optionsBtn.texture, optionsBtn.width, optionsBtn.height, (windowWidth / 2) - optionsBtn.width / 2, 400);
}

void game() {
	movementHandler();

	drawRemotePlayers();
	player.draw(); // Draw current player

	displayText("assets/font/font.ttf", 30, "Hello :)", 150, 150, white);
}

int main(int argc, char* argv[]) {
	Texture someGame = renderText("assets/font/font.ttf", 30, "Some Game :)", white);

	SDL_Surface* title = IMG_Load("assets/textures/title.png");
	SDL_Texture* title_tex = SDL_CreateTextureFromSurface(gameObject.renderer, title);
	Texture titleTex(title_tex, title->h, title->w);
	SDL_FreeSurface(title);

	textures.insert(textureKeyPair("some", someGame));
	textures.insert(textureKeyPair("title", titleTex));

	textures.insert(textureKeyPair("Play", renderText("assets/font/font.ttf", 30, "Play", white)));
	textures.insert(textureKeyPair("Options", renderText("assets/font/font.ttf", 30, "Options", white)));
	client.connect(serverHost);
	client.socket()->emit("user_wants_connection");
	client.socket()->on("user_got_connected", &onConnection);
	client.socket()->on("game_chunk_update", &gameChunkUpdate);
	client.socket()->on("user_disconnect", &handleUserDisconnect);

	while (!connected) {
		//do hee haw
	}

	while (running) {
		calcDeltaTime();

		while (SDL_PollEvent(&event)) {
			eventHandler(event);
		}

		clearScreen();
		game();
		SDL_RenderPresent(gameObject.renderer);
		SDL_Delay(1000 / FPS);
	}
	return 0;
}