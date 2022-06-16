#include "headers.h"
int main(int argc, char* argv[]) {

	Game game(800, 600, "MF"); // Create game (creates window and renderer)

	//Socket connection-y stuff
	Socket socket("http://rpg.json.scot");
	socket.emit("user_wants_connection");
	socket.on("user_got_connected", [&](sio::event &ev) {
		printf("Connected to server\n");
		});


	game.mainTextures.loadTexture("map", "assets/textures/map.png"); //load and store in cache
	TextureObject mapT = game.mainTextures.getTexture("map"); // get from cache
	SDL_Rect textureRect = { 0, 0, mapT.width, mapT.height }; // rect for applying texture

	while (game.isRunning) {
		game.pollEvents(); // poll events (mouse, keys etc)
		game.calculateDeltaTime(); // calculate delta since last frame
		game.clearScreen(); // clear whole screen

		SDL_RenderCopy(game.renderer, mapT.texture, NULL, &textureRect); // apply texture to rect

		game.renderPresent(); // present everything to the renderer

		//Enable this to see mouse position
		//std::cout << "X: " << game.mousePosX << " Y: " << game.mousePosY << std::endl;
	}
	return 0;
}