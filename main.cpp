#include "headers.h"
#include "colours.cpp"
#include <future>

typedef std::_Vector_iterator<std::_Vector_val<std::_Simple_types<Character>>> characterVectorIterator;

std::time_t ms;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;
std::string currentUserSocketId;
sio::client client;

Game game(800, 600, "AoTJ"); // Create game (creates window and renderer)
auto c = game.mainTextures.loadTexture("player", "assets/textures/scot.png");
TextureObject charTexture = game.mainTextures.getTexture("player");
Character player("player", charTexture.texture, 1539, 2076);
extern const int LEVEL_WIDTH = 5832;
extern const int LEVEL_HEIGHT = 4320;
void movementHandler(Socket* socket) {
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (/*keystates[SDL_SCANCODE_LEFT] || */ keystates[SDL_SCANCODE_A]) {
		
		if (player.move("left")) {
			sio::message::list move("l");
			move.push(sio::double_message::create(game.delta));
			move.push(sio::int_message::create(player.rect.x));
			//move.push(sio::int_message::create(ms));
			socket->emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_RIGHT] ||*/ keystates[SDL_SCANCODE_D]) {
		
		if (player.move("right")) {
			sio::message::list move("r");
			move.push(sio::double_message::create(game.delta));
			move.push(sio::int_message::create(player.rect.x));
			//move.push(sio::int_message::create(ms));
			socket->emit("move", move);
		}

	}

	if (/*keystates[SDL_SCANCODE_UP] ||*/ keystates[SDL_SCANCODE_W]) {
		
		if (player.move("up")) {
			sio::message::list move("u");
			move.push(sio::double_message::create(game.delta));
			move.push(sio::int_message::create(player.rect.y));
			//move.push(sio::int_message::create(ms));
			socket->emit("move", move);
		}
	}

	if (/*keystates[SDL_SCANCODE_DOWN] ||*/ keystates[SDL_SCANCODE_S]) {
		
		if (player.move("down")) {
			sio::message::list move("d");
			move.push(sio::double_message::create(game.delta));
			move.push(sio::int_message::create(player.rect.y));
			//move.push(sio::int_message::create(ms));
			socket->emit("move", move);
		}
	}


	// Camera movement (Arrow keys)
	if (keystates[SDL_SCANCODE_UP]) {
		player.camera.y -= (1 * 0.2) * game.delta;

	}

	if (keystates[SDL_SCANCODE_DOWN]) {
		player.camera.y += (1 * 0.2) * game.delta;
	}

	if (keystates[SDL_SCANCODE_LEFT]) {
		player.camera.x -= (1 * 0.2) * game.delta;
	}

	if (keystates[SDL_SCANCODE_RIGHT]) {
		player.camera.x += (1 * 0.2) * game.delta;
	}
}

void gameChunkUpdate(sio::event& evnt) {
	size_t playerChunkSize = evnt.get_message()->get_vector().size();
	playerChunk = evnt.get_message()->get_vector();

	for (int ii = 0; ii < playerChunkSize; ii++) {
		std::string user = evnt.get_message()->get_vector()[ii]->get_map()["socketid"]->get_string();
		int x = evnt.get_message()->get_vector()[ii]->get_map()["x"]->get_int();
		int y = evnt.get_message()->get_vector()[ii]->get_map()["y"]->get_int();

		if (user != currentUserSocketId) {
			characterVectorIterator findUser = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
			if (findUser != playerVector.end()) {
				findUser->pos(x, y);
			}
			else {
				Character newPlayer(user, charTexture.texture);
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
			playerVector[ii].drawRemote(player.camera);
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

int cmdInputToDebugConsole() {
	std::string hello;
	while (std::cin >> hello) {
		if (game.isRunning == false)
			break;
		std::cout << hello << std::endl;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_AUDIO);
	initAudio();
	//playMusic("assets/audio/audio.wav", 5);
	
	//Socket connection-y stuff
	Socket socket("http://rpg.json.scot");
	socket.emit("user_wants_connection");
	socket.on("user_got_connected", [&](sio::event &ev) {
		printf("Connected to server\n");
		});

	socket.on("user_got_connected", &onConnection);
	socket.on("game_chunk_update", &gameChunkUpdate);
	socket.on("user_disconnect", &handleUserDisconnect);
	game.mainTextures.loadTexture("map", "assets/textures/map.png"); //load and store in cache
	
	TextureObject mapT = game.mainTextures.getTexture("map"); // get from cache
	
	SDL_Rect textureRect = { 0,0, mapT.width, mapT.height }; // rect for applying texture

	while (game.isRunning) {
		movementHandler(&socket);
		game.pollEvents(); // poll events (mouse, keys etc)
		game.calculateDeltaTime(); // calculate delta since last frame
		game.clearScreen(); // clear whole screen
		
		//SDL_RenderCopy(game.renderer, mapT.texture, NULL, &textureRect); // apply texture to rect
		
		
		//Enable this to see mouse position
		//std::cout << "X: " << game.mousePosX << " Y: " << game.mousePosY << std::endl;
		

		//player.camera.x = (player.x + player.size / 2) - (game.windowWidth / 2);
		//player.camera.y = (player.y + player.size / 2) - (game.windowHeight / 2);
		if (player.camera.x < 0)
		{
			player.camera.x = 0;
		}
		if (player.camera.y < 0)
		{
			player.camera.y = 0;
		}
		if (player.camera.x > LEVEL_WIDTH - player.camera.w)
		{
			player.camera.x = LEVEL_WIDTH - player.camera.w;
		}
		if (player.camera.y > LEVEL_HEIGHT - player.camera.h)
		{
			player.camera.y = LEVEL_HEIGHT - player.camera.h;
		}

		
		game.clipRenderTexture(mapT.texture, NULL, NULL, 0, 0, &player.camera);
		player.draw();
		drawRemotePlayers();

		std::string mousePos = "Mouse Position - X: " + std::to_string(game.mousePosX) + " Y: " + std::to_string(game.mousePosY);
		std::string charPos = "Character Position - X: " + std::to_string(player.x) + " Y: " + std::to_string(player.y);
		std::string camPos = "Camera Position - X: " + std::to_string(player.camera.x) + " Y: " + std::to_string(player.camera.y);
		std::string fpsText = "FPS: " + std::to_string((int)(game.calcFps()));

		char mousePosBuffer[100];
		char charPosBuffer[100];
		char camPosBuffer[100];
		char fpsBuffer[100];
		strcpy_s(mousePosBuffer, 100, mousePos.c_str());
		strcpy_s(charPosBuffer, 100, charPos.c_str());
		strcpy_s(camPosBuffer, 100, camPos.c_str());
		strcpy_s(fpsBuffer, 100, fpsText.c_str());
		game.displayText("assets/font/font.ttf", 10, mousePosBuffer, 1, 1, black);
		game.displayText("assets/font/font.ttf", 10, charPosBuffer, 1, 15, black);
		game.displayText("assets/font/font.ttf", 10, camPosBuffer, 1, 30, black);
		game.displayText("assets/font/font.ttf", 10, fpsBuffer, 1, 45, black);

		game.render(); // present everything to the renderer

	}
	return 0;
}