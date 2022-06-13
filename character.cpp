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
	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;
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

	//Character() {}
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

	void pos(int xx, int yy) {
		rect.x = xx;
		rect.y = yy;
	}

	void move(std::string direction) {
		
		/*if (x_pos <= 0)
			x_pos = 0;
		if (x_pos >= gameObject.WIDTH - rect.w)
			x_pos = gameObject.WIDTH - rect.w;
		if (y_pos <= 0)
			y_pos = 0;
		if (y_pos >= gameObject.HEIGHT - rect.h)
		{
			y_vel = 0;
			y_pos = gameObject.HEIGHT - rect.h;
		}
		
		if (right_pressed)x_pos += (1 * SPEED) * delta;
		if (left_pressed)x_pos += (-1 * SPEED) * delta;
		if (up_pressed)y_pos += (-1 * SPEED) * delta;
		if (down_pressed)y_pos += (1 * SPEED) * delta;*/
		
		int calc = std::ceil((1 * SPEED) * delta);
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