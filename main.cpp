#include <SDL.h>
#include <iostream>
#include <future>
#include "game.cpp"
#include "character.cpp"
#include <string>
#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>
#include <future>
#include <ctime>
#include <SDL_ttf.h>

#define WINDOW_TITLE "Delta test"
#define SERVER_HOST "http://rpg.json.scot"

int WIDTH = 800;
int HEIGHT = 600;
int FPS = 140;
int ptime = clock();    // Time since last frame
int offset = 0;       // Time since last physics tick
int delta;
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
Character player(gameObject, delta);
SDL_Rect Message_rect;
std::time_t ms;
SDL_Color White = { 255, 50, 152 };
TTF_Font* font;
SDL_Surface* surfaceMessage;
SDL_Texture* Message;


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
            
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if(event.type == SDL_WINDOWEVENT){
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    std::cout << "Resizing window - " << event.window.data1 << event.window.data2 << std::endl;
                    gameObject.resizeWindow(event.window.data1, event.window.data2);
                }
            }
        }
        if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
          
            player.left_pressed = true;
            if (player.left_pressed) {
                player.move(delta, "left");
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
                player.move(delta, "right");
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
                player.move(delta, "up");
                sio::message::list move("u");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.y));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }
        

        if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
            //char* buf = "d";
            
            player.down_pressed = true;
            if (player.down_pressed) {
                player.move(delta, "down");
                sio::message::list move("d");
                move.push(sio::int_message::create(delta));
                move.push(sio::int_message::create(player.rect.y));
                move.push(sio::int_message::create(ms));
                client.socket()->emit("move", move);
            }
        }
        
        if (keystates[SDL_SCANCODE_F]) {
            if (currentFpsSelection == 9) {
                currentFpsSelection = 0;
            }
            else {
                currentFpsSelection += 1;
            }
            FPS = fpsArray[currentFpsSelection];
            std::cout << "FPS changed to " << FPS << std::endl;
        }

        if (keystates[SDL_SCANCODE_C]) {
            player.changeColour();
        }

        if (keystates[SDL_SCANCODE_R]) {
            player.x = 0;
            player.y = 0;
            printf("Position reset");
        }
        
        clearScreen(gameObject.renderer);
        player.draw();
        SDL_RenderCopy(gameObject.renderer, Message, NULL, &Message_rect);
        SDL_RenderPresent(gameObject.renderer);
        SDL_Delay(1000 / FPS);

    }
}

void physLoop() {
    while (running) {
        //printf("Physics Update\n");
        std::cout << "X: " << player.x << "Y: " << player.y << std::endl;
        
        SDL_Delay(15);
    }
}

int main(int argc, char* argv[]) {
    TTF_Init();
    font = font = TTF_OpenFont("assets/font/font.ttf", 40);
    surfaceMessage = TTF_RenderText_Blended_Wrapped(font, "Some Game", White, 0);
    Message = SDL_CreateTextureFromSurface(gameObject.renderer, surfaceMessage);
    TTF_CloseFont(font);
    SDL_FreeSurface(surfaceMessage);
    Message_rect.x = 0;
    Message_rect.y = 0;
    Message_rect.w = surfaceMessage->w;
    Message_rect.h = surfaceMessage->h;
    client.connect(SERVER_HOST);
    client.socket()->emit("user_wants_connection");
    client.socket()->on("user_got_connected", &onEnter);
    SDL_Delay(2000);
    
    auto physicsLoop = std::async(std::launch::async, physLoop);
    gameLoop();
    
    
    //SDL_DestroyTexture(Message);
    return 0;
}