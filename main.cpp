#include "headers.h"
#include "colours.cpp"
#include <future>

typedef std::vector<Character>::iterator characterVectorIterator;

std::time_t ms;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;
std::string currentUserSocketId;
sio::client client;

std::chrono::steady_clock::time_point lastFpsUpdate = std::chrono::steady_clock::now();
double fps = 0;
auto keystates = SDL_GetKeyboardState(NULL);

Game* game = new Game(1280, 720, "AoTJ"); // Create game (creates window and renderer);
Character* player = new Character("player", NULL, 1539, 2076);

extern const int LEVEL_WIDTH = 5832;
extern const int LEVEL_HEIGHT = 4320;
void movementHandler(Socket* socket) {

	if (/*keystates[SDL_SCANCODE_LEFT] || */ keystates[SDL_SCANCODE_A]) {

		if (player->move("left")) {
			//sio::message::list move("l");
			//move.push(sio::double_message::create(game->delta));
			//move.push(sio::int_message::create(player->rect.x));
			//move.push(sio::int_message::create(ms));
			//socket->emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_RIGHT] ||*/ keystates[SDL_SCANCODE_D]) {

		if (player->move("right")) {
			//sio::message::list move("r");
			//move.push(sio::double_message::create(game->delta));
			//move.push(sio::int_message::create(player->rect.x));
			//move.push(sio::int_message::create(ms));
			//socket->emit("move", move);
		}

	}

	if (/*keystates[SDL_SCANCODE_UP] ||*/ keystates[SDL_SCANCODE_W]) {

		if (player->move("up")) {
			//sio::message::list move("u");
			//move.push(sio::double_message::create(game->delta));
			//move.push(sio::int_message::create(player->rect.y));
			//move.push(sio::int_message::create(ms));
			//socket->emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_DOWN] ||*/ keystates[SDL_SCANCODE_S]) {

		if (player->move("down")) {
			//sio::message::list move("d");
			//move.push(sio::double_message::create(game->delta));
			//move.push(sio::int_message::create(player->rect.y));
			//move.push(sio::int_message::create(ms));
			//socket->emit("move", move);
		}
	}


	// Camera movement (Arrow keys)
	// if (keystates[SDL_SCANCODE_UP]) {
	// 	player->camera.y += (-1 * 0.2) * game->delta;

	// }

	// if (keystates[SDL_SCANCODE_DOWN]) {
	// 	player->camera.y += (1 * 0.2) * game->delta;
	// }

	// if (keystates[SDL_SCANCODE_LEFT]) {
	// 	player->camera.x += (-1 * 0.2) * game->delta;
	// }

	// if (keystates[SDL_SCANCODE_RIGHT]) {
	// 	player->camera.x += (1 * 0.2) * game->delta;
	// }
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
			std::cout << playerVector[ii].rect.x << " " << playerVector[ii].rect.x << std::endl;
			playerVector[ii].pos(playerVector[ii].x, playerVector[ii].y);
			playerVector[ii].drawRemote(player->camera);
		}
	}
}

void onConnection(sio::event& evnt) {
	std::string user = evnt.get_message()->get_map()["user"]->get_string();
	printf("Client %s connected\n", user.c_str());
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
	game->mainTextures.loadTexture("player", "./assets/textures/scot.png");
	game->mainTextures.loadTexture("map", "./assets/textures/map.png");

	return true;
}

int main(int argc, char* argv[]) {

	// TODO: Need to figure out why this will not work correctly in the init funciton within the game class
	//SDL_Init(SDL_INIT_AUDIO);
	//initAudio();

	// Load the game textures, will be seperated per scene so we only load what is needed
	bool load = loadTextures();
	if (!load) {
		std::cout << "Error loading textures" << std::endl;
	}

	// Get textures from VRAM and give pointers
	TextureObject charTexture = game->mainTextures.getTexture("player");
	TextureObject mapT = game->mainTextures.getTexture("map");

	SDL_Rect textureRect = { 0,0, mapT.width, mapT.height }; // Rect used for world map (will hoepfully be tilemaps later

	player->applyTexture(charTexture.texture); // Copy texture to player rect

	//playMusic("assets/audio/audio.wav", 5);


	// Socket connection-y stuff
	Socket socket("http://178.128.38.39:4545"); // Connects to server
	socket.emit("user_wants_connection"); // Tell server we want to connect

	socket.on("user_got_connected", &onConnection); // Handle connection success
	socket.on("game_chunk_update", &gameChunkUpdate); // Handle game chunk updates
	socket.on("user_disconnect", &handleUserDisconnect); // Handle remote user DC's

	// Main game loop
	unsigned int a;
	unsigned int b = SDL_GetTicks();
	double d;
	while (game->isRunning) {
		game->pollEvents(); // poll events (mouse, keys etc)
		keystates = SDL_GetKeyboardState(NULL);
		a = SDL_GetTicks();
		d = a - b;
		
		if (d > 1000/64.0){
			b = a;
		
			game->calculateDeltaTime(); // calculate delta since last frame
			//Update FPS counter every 1s
			auto timeSinceLastFpsUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastFpsUpdate).count();
			if (timeSinceLastFpsUpdate > 1000) {
				lastFpsUpdate = std::chrono::steady_clock::now();
				fps = game->calcFps();
			}

			socket.checkLatency(5000);
			movementHandler(&socket);
			
			
			game->clearScreen(); // clear whole screen


			player->camera.x = ((int)player->x + (int)player->size / 2) - (game->windowWidth / 2);
			player->camera.y = ((int)player->y + (int)player->size / 2) - (game->windowHeight / 2);
			if (player->camera.x < 0)
			{
				player->camera.x = 0;
			}
			if (player->camera.y < 0)
			{
				player->camera.y = 0;
			}
			if (player->camera.x > LEVEL_WIDTH - player->camera.w)
			{
				player->camera.x = LEVEL_WIDTH - player->camera.w;
			}
			if (player->camera.y > LEVEL_HEIGHT - player->camera.h)
			{
				player->camera.y = LEVEL_HEIGHT - player->camera.h;
			}


			game->clipRenderTexture(mapT.texture, NULL, NULL, 0, 0, &player->camera);

			drawRemotePlayers();
			player->draw();

			std::string mousePos = "Mouse Position - X: " + std::to_string(game->mousePosX) + " Y: " + std::to_string(game->mousePosY);
			std::string charPos = "Character Position - X: " + std::to_string(player->x) + " Y: " + std::to_string(player->y);
			std::string camPos = "Camera Position - X: " + std::to_string(player->camera.x) + " Y: " + std::to_string(player->camera.y);
			std::string fpsText = "FPS: " + std::to_string(fps);
			std::string chatOpenText = "ChatOpen: " + std::to_string(game->chatOpen);
			std::string latencyText = "Latency: " + std::to_string(socket.latency) + "ms";

			const char *font = "assets/font/font.ttf";
			game->displayText(font, (float)game->windowHeight * 0.02f, charPos, 1, 15, black);
			game->displayText(font, (float)game->windowHeight * 0.02f, mousePos, 1, 1, black);
			game->displayText(font, (float)game->windowHeight * 0.02f, camPos, 1, 30, black);
			game->displayText(font, (float)game->windowHeight * 0.02f, fpsText, 1, 45, black);
			game->displayText(font, (float)game->windowHeight * 0.02f, chatOpenText, 1, 60, black);
			game->displayText(font, (float)game->windowHeight * 0.02f, latencyText, 1, 75, black);

			game->render(); // present everything to the renderer
		}
	}
	return 0;
}
