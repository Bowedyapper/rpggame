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
std::map<std::string, Texture> textures;
typedef std::pair<std::string, Texture> textureKeyPair;
typedef std::_Vector_iterator<std::_Vector_val<std::_Simple_types<Character>>> characterVectorIterator;

double fps;
int seconds = 0;
int frameCount = 0;
double delta = 0.00;
Uint64 timeNow = SDL_GetPerformanceCounter();
Uint64 previousTime = 0;
int offset = 0;
int tickRate = 1000 / 60;
int currentFpsSelection = 0;
int fpsArray[10] = {
	10, 15, 20, 25, 30, 40, 50, 60, 70, 140
};

int mouseXPos;
int mouseYPos;

bool running = true;
bool connected = false;

std::time_t ms;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;
std::string currentUserSocketId;
sio::client client;

Game gameObject;
Character player("player");
SDL_Event event;
TTF_Font* font;


std::string connectingStr = "Connecting to server";
std::string reconnectingStr = "Reconnecting to server";
std::string connectionErrStr = "Could not connect to server";
std::string lostConnToServerErrStr = "Lost connection to server";

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

// Quick and dirty render and yeet
int rendNDestroy(SDL_Texture* texture, SDL_Rect rect) {
	SDL_RenderCopy(gameObject.renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
	return 0;
}

int displayText(char* fontFile, int fontSize, char* text, int x, int y, SDL_Color colour) {
	SDL_Texture* textTexture;
	SDL_Rect textRect;
	Texture texture = renderText(fontFile, fontSize, text, colour);
	textTexture = texture.texture;
	if (x == -1 && y == -1) {
		textRect.x = windowWidth / 2 - texture.width / 2;
		textRect.y = windowHeight / 2 - texture.height / 2;;
	}
	else {
		textRect.x = x;
		textRect.y = y;
	}

	textRect.w = texture.width;
	textRect.h = texture.height;

	return rendNDestroy(texture.texture, textRect);

}

int displayTexture(SDL_Texture* texture, int w, int h, double x, double y, SDL_Rect* clip, double scale) {
	SDL_Rect textureRect;
	textureRect.x = (int)x;
	textureRect.y = (int)y;

	if (clip != NULL)
	{
		textureRect.w = (int)clip->w;
		textureRect.h = (int)clip->h;
	}
	else {
		textureRect.w = w;
		textureRect.h = h;
	}
	return SDL_RenderCopyEx(gameObject.renderer, texture, clip, &textureRect, NULL, NULL, SDL_FLIP_NONE);
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
			FPS = (float)fpsArray[currentFpsSelection];
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

		if (player.move("left")) {
			sio::message::list move("l");
			move.push(sio::double_message::create(delta));
			move.push(sio::int_message::create(player.rect.x));
			move.push(sio::int_message::create(ms));
			client.socket()->emit("move", move);
		}
	}

	if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {

		if (player.move("right")) {
			sio::message::list move("r");
			move.push(sio::double_message::create(delta));
			move.push(sio::int_message::create(player.rect.x));
			move.push(sio::int_message::create(ms));
			client.socket()->emit("move", move);
		}

	}

	if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
		if (player.move("up")) {
			sio::message::list move("u");
			move.push(sio::double_message::create(delta));
			move.push(sio::int_message::create(player.rect.y));
			move.push(sio::int_message::create(ms));
			client.socket()->emit("move", move);
		}
	}

	if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
		if (player.move("down")) {
			sio::message::list move("d");
			move.push(sio::double_message::create(delta));
			move.push(sio::int_message::create(player.rect.y));
			move.push(sio::int_message::create(ms));
			client.socket()->emit("move", move);
		}
	}
}

void calcDeltaTime() {
	previousTime = timeNow;
	timeNow = SDL_GetPerformanceCounter();

	delta = (double)(timeNow - previousTime) / (double)SDL_GetPerformanceFrequency() * 1000.0f;
	ms = std::time(nullptr);
}

void drawRemotePlayers() {
	for (int ii = 0; ii < playerVector.size(); ii++) {
		if (playerVector[ii].socketid != currentUserSocketId) {
			std::cout << playerVector[ii].x << " " << playerVector[ii].y << std::endl;
			playerVector[ii].draw();
		}
	}
}


//TODO: Need to check that the amount of players match the server amount to prune disconnected players
// in the event of a DC or missed packet
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

char* appendCharToCharArray(char* array, char a)
{
	size_t len = strlen(array);

	char* ret = new char[len + 2];

	strcpy_s(ret, len, array);
	ret[len] = a;
	ret[len + 1] = '\0';

	return ret;
}

int iterator = 1;
void connectingScreen(std::string message) {

	size_t len = message.size();
	char connectionString[100];

	if (iterator >= 4) {
		iterator = 1;
	}

	for (int ii = 0; ii < iterator; ii++) {
		message.append(".");
	}



	strcpy(connectionString, message.c_str());
	displayText("assets/font/font.ttf", 30, connectionString, -1, -1, white);
	iterator += 1;
	SDL_Delay(800);
}

void mainMenu() {

	Texture titleImg = getTexture("title");
	Texture playBtn = getTexture("Play");
	Texture optionsBtn = getTexture("Options");



	//displayTexture(titleImg.texture, titleImg.width, titleImg.height, (windowWidth / 2) - titleImg.width / 2, 10);
	//displayTexture(playBtn.texture, playBtn.width, playBtn.height, (windowWidth / 2) - playBtn.width / 2, 350);
	//displayTexture(optionsBtn.texture, optionsBtn.width, optionsBtn.height, (windowWidth / 2) - optionsBtn.width / 2, 400);
}

void game() {
	movementHandler();
	Texture mapT = getTexture("map");
	displayTexture(mapT.texture, mapT.width, mapT.height / 2, 0, 0, &camera, 1);

	player.draw(); // Draw current player
	drawRemotePlayers();

	std::string mousePos = "Mouse Position - X: " + std::to_string(mouseXPos) + " Y: " + std::to_string(mouseYPos);
	std::string charPos = "Character Position - X: " + std::to_string(player.x) + " Y: " + std::to_string(player.y);
	std::string fpsText = "FPS: " + std::to_string((int)(fps));

	char mousePosBuffer[100];
	char charPosBuffer[100];
	char fpsBuffer[100];
	strcpy_s(mousePosBuffer, 100, mousePos.c_str());
	strcpy_s(charPosBuffer, 100, charPos.c_str());
	strcpy_s(fpsBuffer, 100, fpsText.c_str());
	displayText("assets/font/font.ttf", 10, mousePosBuffer, 1, 1, black);
	displayText("assets/font/font.ttf", 10, charPosBuffer, 1, 15, black);
	displayText("assets/font/font.ttf", 10, fpsBuffer, 1, 30, black);
}

void onConnectionHandler() {
	connected = true;
}

void socketDisconnectionHandler() {
	connected = false;
	printf("DISCONNECTED!\n");
}

double calcFps() {
	while (running) {
		fps = (double)1000.0f / (double)delta;
		SDL_Delay(500);
	}
	return 0;
}



void test() {
	movementHandler();

}

int main(int argc, char* argv[]) {
	calcDeltaTime();

	auto nice = std::async(std::launch::async, calcFps);
	Texture someGame = renderText("assets/font/font.ttf", 30, "Some Game :)", white);

	SDL_Surface* gameMap = IMG_Load("assets/textures/map.png");
	SDL_Texture* gameMapTex = SDL_CreateTextureFromSurface(gameObject.renderer, gameMap);
	Texture gameMapC(gameMapTex, gameMap->w, gameMap->h);
	SDL_FreeSurface(gameMap);

	SDL_Surface* title = IMG_Load("assets/textures/title.png");
	SDL_Texture* title_tex = SDL_CreateTextureFromSurface(gameObject.renderer, title);
	Texture titleTex(title_tex, title->h, title->w);
	SDL_FreeSurface(title);

	textures.insert(textureKeyPair("map", gameMapC));
	textures.insert(textureKeyPair("some", someGame));
	textures.insert(textureKeyPair("title", titleTex));

	textures.insert(textureKeyPair("Play", renderText("assets/font/font.ttf", 30, "Play", white)));
	textures.insert(textureKeyPair("Options", renderText("assets/font/font.ttf", 30, "Options", white)));

	client.set_open_listener(std::bind(&onConnectionHandler));
	client.set_socket_close_listener(std::bind(&socketDisconnectionHandler));
	client.set_fail_listener(std::bind(&socketDisconnectionHandler));
	//client.set_reconnect_listener(std::bind(&socketDisconnectionHandler));
	client.connect(serverHost);
	client.socket()->emit("user_wants_connection");
	client.socket()->on("user_got_connected", &onConnection);
	client.socket()->on("game_chunk_update", &gameChunkUpdate);
	client.socket()->on("user_disconnect", &handleUserDisconnect);


	while (running) {
		calcDeltaTime();

		SDL_GetMouseState(&mouseXPos, &mouseYPos);

		
		camera.x = (player.x + player.size / 2) - windowWidth / 2;
		camera.y = (player.y + player.size / 2) - windowHeight / 2;
		//The camera area
		if (camera.x < 0)
		{
			camera.x = 0;
		}
		if (camera.y < 0)
		{
			camera.y = 0;
		}
		if (camera.x > LEVEL_WIDTH - camera.w)
		{
			camera.x = LEVEL_WIDTH - camera.w;
		}
		if (camera.y > LEVEL_HEIGHT - camera.h)
		{
			camera.y = LEVEL_HEIGHT - camera.h;
		}

		clearScreen();

		/*if (!connected) {
			connectingScreen(connectingStr);
		}
		else {
			game();
		}*/

		game();


		SDL_RenderPresent(gameObject.renderer);
		//SDL_Delay(floor(16.666f - delta));

		//fps = (double)1000.0f / (double)delta;
	}
	return 0;
}