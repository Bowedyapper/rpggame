/*****************************************************************//**
 * @file   main.cpp
 * @brief  Contains the main game logic and loop
 * 
 * @author Jason Hewitt <bowedyapper@live.co.uk>
 * @date   June 2022
 * 
 *********************************************************************/

#include "headers.h"

typedef std::vector<Character>::iterator characterVectorIterator;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;
std::string currentUserSocketId;

std::chrono::steady_clock::time_point lastFpsUpdate = std::chrono::steady_clock::now();
double currentCalculatedFrameRate = 0;
double gameFrameRate = 140.00;
extern const int levelWidth = 4320;
extern const int levelHeight = 3360;
auto keystates = SDL_GetKeyboardState(NULL);

Game *game = new Game(1280, 720, "RPGGame"); // Create game (creates window and renderer);
Character* player = new Character("player", NULL, 1064, 1534);

void movementHandler() {
	player->isMoving = false;
	if (/*keystates[SDL_SCANCODE_LEFT] || */ keystates[SDL_SCANCODE_A]) {
		if (player->move("left")) {
			player->isMoving = true;
			sio::message::list move("l");
			move.push(sio::double_message::create(game->delta));
			move.push(sio::int_message::create(player->rect.x));
			//move.push(sio::int_message::create(ms));
			Socket::emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_RIGHT] ||*/ keystates[SDL_SCANCODE_D]) {
		if (player->move("right")) {
			player->isMoving = true;
			sio::message::list move("r");
			move.push(sio::double_message::create(game->delta));
			move.push(sio::int_message::create(player->rect.x));
			//move.push(sio::int_message::create(ms));
			Socket::emit("move", move);
		}

	}

	if (/*keystates[SDL_SCANCODE_UP] ||*/ keystates[SDL_SCANCODE_W]) {
		if (player->move("up")) {
			player->isMoving = true;
			sio::message::list move("u");
			move.push(sio::double_message::create(game->delta));
			move.push(sio::int_message::create(player->rect.y));
			//move.push(sio::int_message::create(ms));
			Socket::emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_DOWN] ||*/ keystates[SDL_SCANCODE_S]) {
		
		if (player->move("down")) {
			player->isMoving = true;
			sio::message::list move("d");
			move.push(sio::double_message::create(game->delta));
			move.push(sio::int_message::create(player->rect.y));
			//move.push(sio::int_message::create(ms));
			Socket::emit("move", move);
		}
	}


	if(game->detachedCamera){
		//Camera movement (Arrow keys)
		int calc = std::round((game->delta / 256) * 50);
		if (keystates[SDL_SCANCODE_UP]) {
			player->camera.y -= calc;

		}

		if (keystates[SDL_SCANCODE_DOWN]) {
			player->camera.y += calc;
		}

		if (keystates[SDL_SCANCODE_LEFT]) {
			player->camera.x -= calc;
		}

		if (keystates[SDL_SCANCODE_RIGHT]) {
			player->camera.x += calc;
		}
	}
}

void gameChunkUpdate(sio::event& evnt) {
	size_t playerChunkSize = evnt.get_message()->get_vector().size();
	playerChunk = evnt.get_message()->get_vector();

	for (int ii = 0; ii < playerChunkSize; ii++) {
		std::string user = evnt.get_message()->get_vector()[ii]->get_map()["socketid"]->get_string();
		float x = (float)evnt.get_message()->get_vector()[ii]->get_map()["x"]->get_double();
		float y = (float)evnt.get_message()->get_vector()[ii]->get_map()["y"]->get_double();

		if (user != currentUserSocketId) {
			characterVectorIterator findUser = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
			if (findUser != playerVector.end()) {
				findUser->pos(x, y);
			}
			else {
				Character newPlayer(user, player->charTexture);
				playerVector.push_back(newPlayer);
			}
		}
	}
}

void drawRemotePlayers() {
	for (int ii = 0; ii < playerVector.size(); ii++) {
		if (playerVector[ii].socketid != currentUserSocketId) {
			playerVector[ii].pos(playerVector[ii].x, playerVector[ii].y);
			playerVector[ii].drawRemote(player->camera);
		}
	}
}

void onConnection(sio::event& evnt) {
	std::string user = evnt.get_message()->get_map()["user"]->get_string();
	utils::debugLog("info", "Player connected with socket id: " + user);
	currentUserSocketId = user;
	Character player(user);
	playerVector.push_back(player);
}

void handleUserDisconnect(sio::event& evnt) {
	std::string user = evnt.get_message()->get_string();
	auto it = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
	if (it != playerVector.end()) {
		auto index = std::distance(playerVector.begin(), it);
		playerVector.erase(playerVector.begin() + index);
	}
}

bool loadTextures() {
	game->mainTextures.loadTexture("map", "./assets/textures/map.png");

	game->mainTextures.loadTexture("playerRightWalk", "./assets/textures/CharWalkRight.png");
	game->mainTextures.loadTexture("playerLeftWalk", "./assets/textures/CharWalkLeft.png");
	game->mainTextures.loadTexture("playerUpWalk", "./assets/textures/CharWalkUp.png");
	game->mainTextures.loadTexture("playerDownWalk", "./assets/textures/CharWalkDown.png");

	return true;
}

void anim(TextureObject textureObject, int x, int y, int w, int h, int speed, int frames, SDL_FRect dstr) {
	int frameWidth = textureObject.width / frames;
	Uint32 ticks = SDL_GetTicks();
	Uint32 seconds = ticks / speed;
	int spriteX = seconds % frames;
	SDL_Rect srcr = {spriteX * frameWidth, 0, frameWidth, textureObject.height};
	//SDL_Rect dstr = {x, y, w, h};
	SDL_RenderCopyF(game->renderer, textureObject.texture, &srcr, &dstr);
}

void extractFrame(TextureObject textureObject, int x, int y, int w, int h, int frame, SDL_FRect dstr) {
	int frameWidth = textureObject.width / frame;
	SDL_Rect srcr = {0, 0, 24, textureObject.height};
	//SDL_Rect dstr = {x, y, w, h};
	SDL_RenderCopyF(game->renderer, textureObject.texture, &srcr, &dstr);
}


int main(int argc, char* argv[]) {
	// TODO: Need to figure out why this will not work correctly in the init function within the game class
	SDL_Init(SDL_INIT_AUDIO);
	initAudio();

	// Load the game textures, will be separated per scene so we only load what is needed
	bool load = loadTextures();
	if (!load) {
		std::cout << "Error loading textures" << std::endl;
	}

	// Get textures from VRAM and give pointers
	TextureObject mapT = game->mainTextures.getTexture("map");

	TextureObject charWalkRight = game->mainTextures.getTexture("playerRightWalk");
	TextureObject charWalkLeft = game->mainTextures.getTexture("playerLeftWalk");
	TextureObject charWalkUp = game->mainTextures.getTexture("playerUpWalk");
	TextureObject charWalkDown = game->mainTextures.getTexture("playerDownWalk");

	TextureObject currentPlayerTexture = charWalkRight;
	SDL_Rect textureRect = { 0,0, mapT.width, mapT.height }; // Rect used for world map (will hopefully be tilemaps later


	playMusic("assets/audio/forest.wav", 10);


	// Socket connection-y stuff

	Socket::connect("http://rpg.json.scot"); // Connects to server
	Socket::emit("user_wants_connection"); // Tell server we want to connect

	Socket::on("user_got_connected", &onConnection); // Handle connection success
	Socket::on("game_chunk_update", &gameChunkUpdate); // Handle game chunk updates
	Socket::on("user_disconnect", &handleUserDisconnect); // Handle remote user DC's


	
	// Main game loop
	unsigned int a;
	unsigned int b = SDL_GetTicks();
	double d;
	while (game->isRunning) {
		game->pollEvents(); // poll events (mouse, keys etc)
		keystates = SDL_GetKeyboardState(NULL);
		a = SDL_GetTicks();
		d = a - b;
		
		if (d > 1000/gameFrameRate){
			b = a;
		
			game->calculateDeltaTime(); // calculate delta since last frame
			//Update FPS counter every 1s
			auto timeSinceLastFpsUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastFpsUpdate).count();
			if (timeSinceLastFpsUpdate > 1000) {
				lastFpsUpdate = std::chrono::steady_clock::now();
				currentCalculatedFrameRate = game->calcFps();
			}

			Socket::checkLatency(5000);
			movementHandler();
			
			
			game->clearScreen(); // clear whole screen


			if(!game->detachedCamera){
				player->camera.x = ((int)player->x + (int)player->size / 2) - (game->windowWidth / 2);
				player->camera.y = ((int)player->y + (int)player->size / 2) - (game->windowHeight / 2);
			}
			
			if (player->camera.x < 0)
			{
				player->camera.x = 0;
			}
			if (player->camera.y < 0)
			{
				player->camera.y = 0;
			}
			if (player->camera.x > levelWidth - player->camera.w)
			{
				player->camera.x = levelWidth - player->camera.w;
			}
			if (player->camera.y > levelHeight - player->camera.h)
			{
				player->camera.y = levelHeight - player->camera.h;
			}

			game->clipRenderTexture(mapT.texture, 0.0, 0, 0, 0, &player->camera);

			drawRemotePlayers();
			player->draw();

			std::string mousePos = "Mouse Position - X: " + std::to_string((int)game->mousePosX) + " Y: " + std::to_string((int)game->mousePosY);
			std::string charPos = "Character Position - X: " + std::to_string((int)player->x) + " Y: " + std::to_string((int)player->y);
			std::string camPos = "Camera Position - X: " + std::to_string(player->camera.x) + " Y: " + std::to_string(player->camera.y);
			std::string fpsText = "FPS: " + std::to_string((int)currentCalculatedFrameRate);
			std::string latencyText = "Latency: " + std::to_string(Socket::latency) + "ms";

			const char *font = "assets/font/font.ttf";
			game->displayText(font, (float)game->windowHeight * 0.02f, charPos, 1, 15, darker_gray);
			game->displayText(font, (float)game->windowHeight * 0.02f, mousePos, 1, 1, darker_gray);
			game->displayText(font, (float)game->windowHeight * 0.02f, camPos, 1, 30, darker_gray);
			game->displayText(font, (float)game->windowHeight * 0.02f, fpsText, 1, 45, darker_gray);
			game->displayText(font, (float)game->windowHeight * 0.02f, latencyText, 1, 60, darker_gray);


			// Uint32 ticks = SDL_GetTicks();
			// Uint32 seconds= ticks / 50;
			// int spriteX = seconds % 4;
			
			// SDL_Rect srcr = {spriteX *24 ,0, 25,20};
			// SDL_Rect dstr = {game->windowWidth/2, game->windowHeight/2, 70, 70};
			// SDL_RenderCopy(game->renderer, charWalkRight.texture, &srcr, &dstr);

			
			//void anim(TextureObject textureObject, int x, int y, int w, int h, int speed, int frames, int frameOffset) 

			
			if(player->facingDirection == "r"){
				currentPlayerTexture = charWalkRight;
			}
			if(player->facingDirection == "l"){
				currentPlayerTexture = charWalkLeft;
			}
			if(player->facingDirection == "u"){
				currentPlayerTexture = charWalkUp;
			}
			if(player->facingDirection == "d"){
				currentPlayerTexture = charWalkDown;
			}
			if(player->isMoving){
				anim(currentPlayerTexture, game->windowWidth/2, game->windowHeight/2, 200, 200, 150, 4, player->rect);
			} else {
				extractFrame(currentPlayerTexture, game->windowWidth / 2, game->windowHeight / 2, 100, 200, 1, player->rect);
			}
			game->render(); // present everything to the renderer
		}
	}
	return 0;
}


