#include <sio_client.h>
#include <sio_message.h>
#include <sio_socket.h>
class Character {
private: 
	Game gameObject;
public:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;
	float SPEED = 0.3;
	int x_pos, y_pos = 0;
	int x_vel, y_vel = 0;
	float x = 0;
	float y = 0;
	int size = 50;
	int currentColour = 0;
	std::string colourArrayLabel[4] = {
		"White",
		"Orange",
		"Red",
		"Blue"
	};

	int colourArray[4][4] = {
	   {255, 255, 255, 255},
	   {252, 168, 3, 255},
	   {235, 64, 52, 255},
	   {69, 3, 252, 255}
	};

	Character() {}
	Character(Game& gameObj, int delta) {
		gameObject = gameObj;
		renderer = gameObj.renderer;
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

	void move(int delta, std::string direction) {
		
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
		rect.x = (int)x;
		rect.y = (int)y;

		if (direction == "up" && up_pressed) {
			y -= (1 * SPEED) * delta;
			
		}

		if (direction == "down" && down_pressed) {
			y += (1 * SPEED) * delta;
			
			
		}

		if (direction == "left" && left_pressed) {
			x -= (1 * SPEED) * delta;
			
			
		}

		if (direction == "right" && right_pressed) {
			x += (1 * SPEED) * delta;
			
		}
		up_pressed = false;
		down_pressed = false;
		left_pressed = false;
		right_pressed = false;
	}

	void draw() {
		/* Draw the rectangle */
		SDL_SetRenderDrawColor(renderer, colourArray[currentColour][0], colourArray[currentColour][1], colourArray[currentColour][2], colourArray[currentColour][3]);
		SDL_RenderFillRect(renderer, &rect);
		SDL_RenderDrawRect(renderer, &rect);
	};
};