#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>
extern Game gameObject;
extern double delta;
class Character {
private:
public:
	SDL_Rect rect;

	SDL_Rect imgRect;

	SDL_Renderer* renderer;
	double SPEED = 0.3;
	int size = 50;
	double x = 0;
	double y = 0;
	
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

	Character(std::string socketId) {
		socketid = socketId;
		renderer = gameObject.renderer;
		rect = { 0, 0, size, size };

	};

	void changeColour() {
		if (currentColour == 3) {
			currentColour = 0;
		}
		else {
			currentColour += 1;
		}

		std::cout << "Colour changed to " << colourArrayLabel[currentColour] << std::endl;
	}

	void pos(double xPos, double yPos) {
		rect.x = (int)xPos - camera.x;
		rect.y = (int)yPos - camera.y;
	}

	bool move(std::string direction) {
		double calc = std::ceil((1 * SPEED) * delta);

		//If the dot went too far to the left or right
		if ((x < 0) || (x + size > LEVEL_WIDTH))
		{
			//Move back
			double calc = -1;
			return false;
		} else if ((y < 0) || (y + size > LEVEL_HEIGHT))
		{
			//Move back
			double calc = -1;
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
			}

			if (direction == "right") {
				x += calc;
			}
		}

		rect.x = x - camera.x;
		rect.y = y - camera.y;

		return true;
	}

	void draw() {

		/* Draw the rectangle */
		SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderDrawRect(renderer, &rect);
	

	};
};