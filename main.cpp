#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <algorithm>
#include <iostream>
#include "game.cpp"
#include "character.cpp"
#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>

int connectedUsersSize = 0;
int fontSize = 40;
int currentUsersOnServer;
int mousePosX;
int mousePosY;
int curUserPosInArray;
int WIDTH = 500;
int HEIGHT = 500;
bool connected = false;
bool sentInitial = false;
char* WINDOW_TITLE = "Oshi";
std::string curUser;
Character connectedUsers[3];
SDL_Color White = { 255, 255, 255 };
SDL_Event event;
Game gameObject(WIDTH, HEIGHT);
sio::client client;

void onEnter(sio::event& e) {
    std::string u = e.get_message()->get_map()["user"]->get_string();
    printf("Client %s connected\n", u.c_str());
    curUser = u;
    connected = true;
}

void gameTick(sio::event& e) {
    sio::message::list user;
    user.push(sio::string_message::create(std::to_string(mousePosX)));
    user.push(sio::string_message::create(std::to_string(mousePosY)));
    client.socket()->emit("user_update", user);
    int checkCurrentUsersOnServer = e.get_message()->get_vector().size();
    if (checkCurrentUsersOnServer != currentUsersOnServer) {
        currentUsersOnServer = checkCurrentUsersOnServer;
    }

    for (int i = 0; i < checkCurrentUsersOnServer; i++) {
        std::string user = e.get_message()->get_vector()[i]->get_map()["user"]->get_string();
        std::string x = e.get_message()->get_vector()[i]->get_map()["x"]->get_string();
        std::string y = e.get_message()->get_vector()[i]->get_map()["y"]->get_string();
        if (user == curUser) {
            curUserPosInArray = i;
            connectedUsers[i] = Character(gameObject.renderer);
            connectedUsers[i].isPlayer == true;
            connectedUsers[i].move(std::stoi(x), std::stoi(y));
        }
        else {
            connectedUsers[i] = Character(gameObject.renderer);
            connectedUsers[i].move(std::stoi(x), std::stoi(y));
        }
    }
}

int main(int argc, char* argv[]) {
    gameObject.setTitle(WINDOW_TITLE);
    int TTF_Engine = TTF_Init();
    if (!TTF_Engine) {
        printf("Could not init TTF Engine");
    }

    TTF_Font* font = TTF_OpenFont("assets/font/font.ttf", fontSize);
    if (!font) {
        printf("Could not open font file");
    }

    
    
    client.connect("http://rpg.json.scot");
    client.socket()->emit("user_wants_connection");
    client.socket()->on("user_got_connected", &onEnter);
    SDL_Delay(1000);
    SDL_GetMouseState(&mousePosX, &mousePosY);
    while (!sentInitial) {
        
        
        if (connected) {
            printf("HELLO");
            sio::message::list user(std::to_string(mousePosX));
            user.push(sio::string_message::create(std::to_string(mousePosY)));
            client.socket()->emit("send_inital_user_data", user);

            connectedUsers[0] = Character(gameObject.renderer);
            connectedUsers[0].isPlayer = true;
            connectedUsers[0].rect.x = mousePosX;
            connectedUsers[0].rect.y = mousePosY;
            sentInitial = true;
            break;
        }
    }
    
    client.socket()->on("game_tick", &gameTick);

    bool running = true;
    
    while (running) {
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
            case SDL_QUIT:
                printf("Game was closed");
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {

                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        SDL_SetRenderDrawColor(gameObject.renderer, 0, 0, 0, 255);
        SDL_RenderClear(gameObject.renderer);
        
        
        SDL_GetMouseState(&mousePosX, &mousePosY);
        std::string text = "Mouse - X: " + std::to_string(mousePosX) + " Y: " + std::to_string(mousePosY);

        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), White);
        SDL_Texture* Message = SDL_CreateTextureFromSurface(gameObject.renderer, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = 0;  //controls the rect's x coordinate 
        Message_rect.y = 0; // controls the rect's y coordinte
        Message_rect.w = strlen(text.c_str()) * 10 + fontSize; // controls the width of the rect
        Message_rect.h = fontSize; // controls the height of the rect

        connectedUsers[curUserPosInArray].move(mousePosX, mousePosY);
        connectedUsers[curUserPosInArray].draw();

        for (int i = 0; i < currentUsersOnServer; i++) {
            if (!connectedUsers[i].isPlayer) {
                connectedUsers[i].draw();
            }
        }
        SDL_RenderCopy(gameObject.renderer, Message, NULL, &Message_rect);

        SDL_DestroyTexture(Message);
        SDL_FreeSurface(surfaceMessage);
        SDL_RenderPresent(gameObject.renderer);
        SDL_Delay(1000 / 60);
    }
    SDL_DestroyRenderer(gameObject.renderer);
    SDL_DestroyWindow(gameObject.window);
    SDL_Quit();
    return 0;
}