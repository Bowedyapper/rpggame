#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>
extern Game gameObject;
extern int delta;
class Character {
private:
public:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	double SPEED = 0.3;
	double x = 0;
	double y = 0;
	int size = 50;
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
		rect = { static_cast<int>(x), static_cast<int>(y), size, size };

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
		rect.x = xPos;
		rect.y = yPos;
	}

	void move(std::string direction) {

		double calc = std::ceil((1 * SPEED) * delta);
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

		rect.x = (int)x;
		rect.y = (int)y;


	}

	void draw() {
		/* Draw the rectangle */
		SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderDrawRect(renderer, &rect);
	};
};