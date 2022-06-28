/*****************************************************************//**
 * \file   character.h
 * \brief  Creates a player object and stores current x, y coordinates
 * 		   size, speed, texture and also handles player movement.
 * 		   This class is also used for rendering and positioning of
 * 		   remote players
 * 
 * \author Jason Hewitt <bowedyapper@live.co.uk>
 * \date   June 2022
 *********************************************************************/

extern Game *game;
class Character {
private:
public:

	float SPEED = 50;
	float size = 50;
	float x;
	float y;

	float delta;

	SDL_Renderer* renderer;
	SDL_Texture* charTexture;
	SDL_FRect rect = { x, y, size, size };
	SDL_Rect camera = { 1400, 1900, game->windowWidth, game->windowHeight };
	SDL_Rect imgRect;

	SDL_RendererFlip facingDirection = SDL_FLIP_HORIZONTAL;

	std::string direction;
	int currentColour = 0;
	std::string colourArrayLabel[4] = {
		"White",
		"Orange",
		"Red",
		"Blue"
	};
	std::string socketid;

	int colourArray[4][4] = {
	   {255, 255, 255, 255},
	   {252, 168, 3, 255},
	   {235, 64, 52, 255},
	   {69, 3, 252, 255}
	};

	Character(std::string socketId = "player", SDL_Texture* charTex = nullptr, float xPos = 0, float yPos = 0) {
		renderer = game->renderer;
		socketid = socketId;
		charTexture = charTex;
		x = xPos;
		y = yPos;
		rect.x = (x - camera.x);
		rect.y = (y - camera.y);

	};

	void changeColour();
	void pos(float xPos, float yPos);
	bool move(std::string direction);
	void draw();
	void drawRemote(SDL_Rect playerCamera);
	void applyTexture(SDL_Texture* texture);
};


void Character::changeColour() {
	if (currentColour == 3) {
		currentColour = 0;
	}
	else {
		currentColour += 1;
	}

	std::cout << "Colour changed to " << colourArrayLabel[currentColour] << std::endl;
}

void Character::pos(float xPos, float yPos) {
	x = xPos;
	y = yPos;
	rect.x = xPos - camera.x;
	rect.y = yPos - camera.y;
}

bool Character::move(std::string direction) {
	float calc = (float)std::round((game->delta / 256) * SPEED);

	//If the player went too far to the left or right
	if ((x < 0) || (x + size > 6336))
	{
		//Move back
		x += calc;
		return false;
	}
	else if ((y < 0) || (y + size > 4442))
	{
		//Move back
		y += calc;
		return false;
	}
	else {

		if (direction == "up") {
			y -= calc;
		}

		if (direction == "down") {
			y += calc;
		}

		if (direction == "left") {
			x -= calc;
			facingDirection = SDL_FLIP_NONE;
		}

		if (direction == "right") {
			x += calc;
			facingDirection = SDL_FLIP_HORIZONTAL;
		}
	}


	return true;
}

void Character::drawRemote(SDL_Rect playerCamera) {
	rect.x = (x - playerCamera.x);
	rect.y = (y - playerCamera.y);
	if (charTexture) {
		SDL_RenderCopyExF(renderer, charTexture, NULL, &rect, NULL, NULL, facingDirection);
		//SDL_RenderCopy(renderer, charTexture, NULL, &rect);
	}
	else {
		SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
		SDL_RenderFillRectF(renderer, &rect);
		SDL_RenderDrawRectF(renderer, &rect);
	}
}

void Character::draw() {
	rect.x = (x - camera.x);
	rect.y = (y - camera.y);
	/* Draw the rectangle */

	if (charTexture) {
		SDL_RenderCopyExF(renderer, charTexture, NULL, &rect, NULL, NULL, facingDirection);
		//SDL_RenderCopy(renderer, charTexture, NULL, &rect);
	}
	else {
		SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
		SDL_RenderFillRectF(renderer, &rect);
		SDL_RenderDrawRectF(renderer, &rect);
	}
};

void Character::applyTexture(SDL_Texture* texture) {
	charTexture = texture;
}
