#include "headers.h"
SDL_Event event;
int main(int argc, char* argv[]) {
	Game game(800, 600, "MF");
	Socket socket("http://rpg.json.scot");
	socket.emit("user_wants_connection");
	socket.on("user_got_connected", [&](sio::event &ev) {
		printf("Connected mf\n");
		});


	//game.mainTextures.loadTexture("map", "assets/textures/map.png");
	
	//TextureObject mapT = game.mainTextures.getTexture("map");
	//SDL_Rect textureRect = { 0, 0, mapT.width, mapT.height };
	while (game.isRunning) {
		while (SDL_PollEvent(&event)) {
			game.eventHandler(event);
		}
		game.calculateDeltaTime();
		game.clearScreen();

		//SDL_RenderCopy(game.renderer, mapT.texture, NULL, &textureRect);

		game.renderPresent();
		
	}
	return 0;
}