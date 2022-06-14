#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <future>
#include <string>
#include <future>
#include <ctime>
//#include <map>
#include "globals.h"
#include "colours.cpp"
#include "game.cpp"
#include "character.cpp"
#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

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
Game gameObject(windowWidth, windowHeight);
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

auto renderText(char* fontFile, int fontSize, char* text, int x, int y) {
	SDL_Surface* textSurface;
	SDL_Texture* textTexture;
	SDL_Rect textRect;
	font = TTF_OpenFont(fontFile, fontSize);
	textSurface = TTF_RenderText_Blended_Wrapped(font, text, black, 0);
	textTexture = SDL_CreateTextureFromSurface(gameObject.renderer, textSurface);
	textRect.x = x;
	textRect.y = y;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;
	TTF_CloseFont(font);
	SDL_FreeSurface(textSurface);
	return SDL_RenderCopy(gameObject.renderer, textTexture, NULL, &textRect);

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
			std::_Vector_iterator<std::_Vector_val<std::_Simple_types<Character>>> findUser = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
			if (findUser != playerVector.end()) {
				findUser->pos(x, y);
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


int main(int argc, char* argv[]) {
	
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

		movementHandler();

		clearScreen();

		drawRemotePlayers();
		player.draw(); // Draw current player

		renderText("assets/font/font.ttf", 30, "Hello :)", 150, 150);
		SDL_RenderPresent(gameObject.renderer);
		SDL_Delay(1000 / FPS);
	}
	return 0;
}