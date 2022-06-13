#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <future>
#include <string>
#include <future>
#include <ctime>
#include "game.cpp"
#include "character.cpp"
#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

#define WINDOW_TITLE "Some RPG Game of some sort :\\"
#define SERVER_HOST "http://rpg.json.scot"

int WIDTH = 800;
int HEIGHT = 600;
int FPS = 140;
int delta;
int ptime = clock();    // Time since last frame
int offset = 0;       // Time since last physics tick
int tickRate = 1000 / 60;
int currentFpsSelection = 0;
int fpsArray[10] = {
    10, 15, 20, 25, 30, 40, 50, 60, 70, 140
};
bool running = true;
bool connected = false;
std::string currentUserSocketId;

Game gameObject(WIDTH, HEIGHT);
sio::client client;
SDL_Event event;
Character player("player");
SDL_Rect Message_rect;
std::time_t ms;
SDL_Color White = { 255, 50, 152 };
TTF_Font* font;
SDL_Surface* surfaceMessage;
SDL_Texture* Message;
std::vector<Character> playerVector;
std::vector<sio::message::ptr> playerChunk;


int deltaTime(int previous, int offset) {
    int Δt = (clock() - previous);  // Δt = Current time - time of last frame
    return Δt;
}

void clearScreen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void onEnter(sio::event& evnt) {
    std::string user = evnt.get_message()->get_map()["user"]->get_string();
    printf("Client %s connected\n", user.c_str());
    currentUserSocketId = user;
    connected = true;
}

void gameLoop() {
    while (running) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        delta = deltaTime(ptime, offset);
        ptime = clock();
        offset = delta % (1000 / tickRate);
        ptime = clock();
        ms = std::time(nullptr);

        while (SDL_PollEvent(&event)) {
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

        if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {

            player.left_pressed = true;
            if (player.left_pressed) {
                player.move("left");
                sio::message::list move("l");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.x));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }

        if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {

            player.right_pressed = true;
            if (player.right_pressed) {
                player.move("right");
                sio::message::list move("r");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.x));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }

        if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {

            player.up_pressed = true;
            if (player.up_pressed) {
                player.move("up");
                sio::message::list move("u");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.y));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }

        if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
            player.down_pressed = true;
            if (player.down_pressed) {
                player.move("down");
                sio::message::list move("d");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.y));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }
        clearScreen(gameObject.renderer);
        for (int ii = 0; ii < playerVector.size(); ii++) {
            //std::cout << playerVector[ii].socketid << std::endl;
            if (playerVector[ii].socketid != "player") {
                playerVector[ii].draw();
            }
        }
        player.draw();
        SDL_RenderCopy(gameObject.renderer, Message, NULL, &Message_rect);
        SDL_RenderPresent(gameObject.renderer);
        SDL_Delay(1000 / FPS);
    }
    
}


void gameTick(sio::event& evnt) {
    size_t playerChunkSize = evnt.get_message()->get_vector().size();
    playerChunk = evnt.get_message()->get_vector();
    
    for (int ii = 0; ii < playerChunkSize; ii++) {
        std::string cunt = "player";
        std::string user = evnt.get_message()->get_vector()[ii]->get_map()["socketid"]->get_string();
        double x = evnt.get_message()->get_vector()[ii]->get_map()["x"]->get_double();
        double y = evnt.get_message()->get_vector()[ii]->get_map()["y"]->get_double();

        if (user != currentUserSocketId) {
            auto it = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
            if (it != playerVector.end())
            {

                it->pos(x, y);
                //std::cout << it->x << std::endl;
            }
            else {
                Character newPlayer(user);
                playerVector.push_back(newPlayer);
            }
        }

        //std::cout << user << " @ X:" << x << " Y: " << y << std::endl;
    }
}

void handleUserLeft(sio::event& evnt) {
    std::string user = evnt.get_message()->get_string();
    auto it = find_if(playerVector.begin(), playerVector.end(), [&user](const Character obj) {return obj.socketid == user; });
    if (it != playerVector.end())
    {
        auto index = std::distance(playerVector.begin(), it);
        playerVector.erase(playerVector.begin() + index);
    }
}
int main(int argc, char* argv[]) {
    playerVector.push_back(player);
    //TTF_Init();
    //font = TTF_OpenFont("assets/font/font.ttf", 40);
   // surfaceMessage = TTF_RenderText_Blended_Wrapped(font, "", White, 0);
    //Message = SDL_CreateTextureFromSurface(gameObject.renderer, surfaceMessage);
    //TTF_CloseFont(font);
    //SDL_FreeSurface(surfaceMessage);
    //Message_rect.x = 0;
    //Message_rect.y = 0;
    //Message_rect.w = surfaceMessage->w;
    //Message_rect.h = surfaceMessage->h;
    client.connect(SERVER_HOST);
    client.socket()->emit("user_wants_connection");
    client.socket()->on("user_got_connected", &onEnter);
    client.socket()->on("game_tick", &gameTick);
    client.socket()->on("user_left", &handleUserLeft);
    SDL_Delay(50);
   
    gameLoop();
    
    
    //SDL_DestroyTexture(Message);
    return 0;
}