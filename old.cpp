#include <fstream>
#include <iostream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "include/audio.c"
#include "include/audio.h"

#define WINDOW_TITLE "Holy shit, its a game written in C!"
#define WIDTH 1920
#define HEIGHT 1080
#define SIZE 50
#define SPEED 500
#define GRAVITY 50
#define FPS 60
#define JUMP -3000
void dev(char *text) {
    std::cout << text;
}
int main(int argc, char *argv[]){
   printf("Loading Game");
#if EE_CURRENT_PLATFORM == EE_PLATFORM_WINDOWS
    SDL_setenv("SDL_AUDIODRIVER", "directsound", true);
#endif
    /* Initializes the timer, audio, video, joystick,
    haptic, gamecontroller and events subsystems */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }
    /* CDreate a window */
    SDL_Window* wind = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, 0);
    if (!wind)
    {
        printf("Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }
    SDL_SetWindowBordered(wind, SDL_FALSE);
    /* Create a renderer */
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);
    if (!rend)
    {
        printf("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return 0;
    }
    
    SDL_Surface* image = SDL_LoadBMP("assets/textures/jord.bmp");
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, image);
    SDL_FreeSurface(image);
    /* Main loop */
    bool running = true, jump_pressed = false, can_jump = true,
        left_pressed = false, right_pressed = false, up_pressed = false, down_pressed = false;
    int x_pos = (WIDTH - SIZE) / 2, y_pos = (HEIGHT - SIZE) / 2, x_vel = 0, y_vel = 0;
    SDL_Rect rect = { (int)x_pos, (int)y_pos, SIZE, SIZE };
    SDL_Event event;
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1;
    }
    initAudio();
    playMusic("assets/audio/audio.wav", SDL_MIX_MAXVOLUME);
    TTF_Init();
    int fontSize = 40;
    TTF_Font* font = TTF_OpenFont("assets/font/font.ttf", fontSize);
    if (!font) {
        printf("Could not open font file");
        return(2);
    }
    SDL_Color White = { 255, 255, 255 };
    int mousePosX;
    int mousePosY;
    
 
    while (running){
        /* Process events */
        while (SDL_PollEvent(&event))
        {
            switch (event.type){
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode){
                case SDL_SCANCODE_SPACE:
                    jump_pressed = true;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left_pressed = true;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right_pressed = true;
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up_pressed = true;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down_pressed = true;
                    break;
                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode){
                case SDL_SCANCODE_SPACE:
                    jump_pressed = false;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left_pressed = false;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right_pressed = false;
                    break;
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    up_pressed = false;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    down_pressed = false;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        
        // Clear screen
        
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        SDL_RenderClear(rend);
        
        // Move the rectangle

        /*x_vel = (right_pressed - left_pressed) * SPEED;
        y_vel += GRAVITY;
        if (jump_pressed && can_jump)
        {
            can_jump = false;
            y_vel = JUMP;
        }
        x_pos += x_vel / 60;
        y_pos += y_vel / 60;
        if (x_pos <= 0)
            x_pos = 0;
        if (x_pos >= WIDTH - rect.w)
            x_pos = WIDTH - rect.w;
        if (y_pos <= 0)
            y_pos = 0;
        if (y_pos >= HEIGHT - rect.h)
        {
            y_vel = 0;
            y_pos = HEIGHT - rect.h;
            if (!jump_pressed)
                can_jump = true;
        }*/
        if (x_pos >= (WIDTH - rect.w) / 2) {
            x_vel = 0;
           x_pos =(WIDTH - rect.w) / 2;
        }

        if (y_pos >= (HEIGHT - rect.h) /2) {
            std::cout << std::to_string(HEIGHT - rect.h / 2) << "\n";
            y_vel = 0;
            y_pos = (HEIGHT - rect.h) /2;
        }
        x_vel = (right_pressed - left_pressed) * SPEED;
        y_vel = (down_pressed - up_pressed) * SPEED;
        x_pos += x_vel / 60;
        y_pos += y_vel / 60;
        rect.x = (int)x_pos;
        rect.y = (int)y_pos;

        SDL_GetMouseState(&mousePosX, &mousePosY);
        std::string text = "Mouse - X: " + std::to_string(mousePosX) + " Y: " + std::to_string(mousePosY) + " Char - X: " + std::to_string(x_pos) + " Y: " + std::to_string(y_pos);
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), White);
        SDL_Texture* Message = SDL_CreateTextureFromSurface(rend, surfaceMessage);
        SDL_Rect Message_rect; //create a rect
        Message_rect.x = 0;  //controls the rect's x coordinate 
        Message_rect.y = 0; // controls the rect's y coordinte
        Message_rect.w = strlen(text.c_str()) * 10 + fontSize; // controls the width of the rect
        Message_rect.h = fontSize; // controls the height of the rect

        /* Draw the rectangle */
        SDL_RenderCopy(rend, texture, NULL, &rect);
        SDL_RenderCopy(rend, Message, NULL, &Message_rect);

        /* Draw to window and loop */
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / FPS);
    }
    /* Release resources */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}
