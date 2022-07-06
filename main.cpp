#include "headers.h"

int main(int argc, char *argv[]) {
    Game::createGame(1280, 720, "RPGGame");
    Game::createGame(1280, 720, "RPGGame");
    playMusic("assets/audio/forest.wav", 100);
    Game::mainTextures.loadTexture("map", "./assets/textures/map.png");
    TextureObject mapT = Game::mainTextures.getTexture("map");
    SDL_Rect mapRect = {0, 0, mapT.width, mapT.height};

    while (Game::isRunning) {
        Game::pollEvents();
        const char *font = "assets/font/font.ttf";
        
        SDL_RenderCopy(Game::renderer, mapT.texture, NULL, &mapRect);
        Game::displayText(font, (float)Game::windowHeight * 0.1f, utils::generateRandomString(10), -1, -1, darker_gray);
        Game::render();
        SDL_Delay(500);
    }
    return 0;
};