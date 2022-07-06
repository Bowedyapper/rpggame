/*****************************************************************/ /**
 * @file   character.h
 * @brief  Creates a player object and stores current x, y coordinates
 * 		   size, speed, texture and also handles player movement.
 * 		   This class is also used for rendering and positioning of
 * 		   remote players
 * 
 * @author Jason Hewitt <bowedyapper@live.co.uk>
 * @date   June 2022
 *********************************************************************/

extern Game *game;

class Character {
private:
public:
    float SPEED = 48;
    float size = 80;
    float x;
    float y;

    float delta;
    bool isMoving = false;
    std::chrono::steady_clock::time_point lastDataDump = std::chrono::steady_clock::now();
    SDL_Renderer *renderer;
    SDL_Texture *charTexture;
    SDL_FRect rect = {x, y, size, size};
    SDL_Rect camera = {1400, 1900, game->windowWidth, game->windowHeight};
    SDL_Rect imgRect;

    std::string facingDirection = "r";

    std::string direction;

    std::string socketid;

    int colourArray[4][4] = {
        {255, 255, 255, 255},
        {252, 168, 3, 255},
        {235, 64, 52, 255},
        {69, 3, 252, 255}};

    Character(std::string socketId = "player", SDL_Texture *charTex = nullptr, float xPos = 0, float yPos = 0) {
        renderer = game->renderer;
        socketid = socketId;
        charTexture = charTex;
        x = xPos;
        y = yPos;
        rect.x = (x - camera.x);
        rect.y = (y - camera.y);
    };

    void pos(float xPos, float yPos);
    bool move(std::string direction);
    void draw();
    void drawRemote(SDL_Rect playerCamera);
    void applyTexture(SDL_Texture *texture);
    void dumpDataToServer(int interval, Character character);
};

void Character::pos(float xPos, float yPos) {
    x = xPos;
    y = yPos;
    rect.x = xPos - camera.x;
    rect.y = yPos - camera.y;
}

bool Character::move(std::string direction) {
    float calc = (float)std::round((game->delta / 256) * SPEED);

    //If the player went too far to the left or right
    if ((x < 0) || (x + size > 6336)) {
        //Move back
        x += calc;
        return false;
    } else if ((y < 0) || (y + size > 4442)) {
        //Move back
        y += calc;
        return false;
    } else {

        if (direction == "up") {
            y -= calc;
            facingDirection = "u";
        }

        if (direction == "down") {
            y += calc;
            facingDirection = "d";
        }

        if (direction == "left") {
            x -= calc;
            facingDirection = "l";
        }

        if (direction == "right") {
            x += calc;
            facingDirection = "r";
        }
    }

    return true;
}

void Character::drawRemote(SDL_Rect playerCamera) {
    rect.x = (x - playerCamera.x);
    rect.y = (y - playerCamera.y);
    // if (charTexture) {
    // 	SDL_RenderCopyExF(renderer, charTexture, NULL, &rect, 0, nullptr, facingDirection);
    // 	//SDL_RenderCopy(renderer, charTexture, NULL, &rect);
    // }
    // else {
    // 	SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
    // 	SDL_RenderFillRectF(renderer, &rect);
    // 	SDL_RenderDrawRectF(renderer, &rect);
    // }
}

void Character::draw() {
    rect.x = (x - camera.x);
    rect.y = (y - camera.y);
    /* Draw the rectangle */
    // if (charTexture) {
    // 	SDL_RenderCopyExF(renderer, charTexture, NULL, &rect, 0, NULL, facingDirection);
    // 	//SDL_RenderCopy(renderer, charTexture, NULL, &rect);
    // }
    // else {
    // 	SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
    // 	SDL_RenderFillRectF(renderer, &rect);
    // 	SDL_RenderDrawRectF(renderer, &rect);
    // }
};

void Character::applyTexture(SDL_Texture *texture) {
    charTexture = texture;
}

void Character::dumpDataToServer(int interval, Character cr) {

    int64_t elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastDataDump).count();
    if (elapsed_time > interval) {
        //sio::message::list characterData("s");
        //socket->emit("characterData", characterData);
    }
}