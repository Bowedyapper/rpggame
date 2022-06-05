
class Character {
private:
public:
	SDL_Rect rect;
	SDL_Renderer* renderer;
	bool isPlayer = false;
	Character() {}
	Character(SDL_Renderer* rend) {
		renderer = rend;
		rect = { 0,0, 100, 100 };
	};

	void move(int x, int y) {
		rect.x = x;
		rect.y = y;
	}

	void draw() {
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
		SDL_RenderDrawRect(renderer, &rect);
	};
};