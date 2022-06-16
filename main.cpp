#include "headers.h"
int main(int argc, char* argv[]) {
	Game game(800, 600, "MF");
	Socket socket("http://rpg.json.scot");
	socket.emit("user_wants_connection");
	socket.on("user_got_connected", [&](sio::event &ev) {
		printf("Connected mf\n");
		});


	game.mainTextures.loadTexture("map", "assets/textures/map.png");
	
	TextureObject mapT = game.mainTextures.getTexture("map");
	SDL_Rect textureRect = { 0, 0, mapT.width, mapT.height };
	while (game.isRunning) {
		game.pollEvents();
		game.calculateDeltaTime();
		game.clearScreen();

		SDL_RenderCopy(game.renderer, mapT.texture, NULL, &textureRect);

		game.renderPresent();

		//Enable this to see mouse position
		//std::cout << "X: " << game.mousePosX << " Y: " << game.mousePosY << std::endl;
		
	}
	return 0;
}