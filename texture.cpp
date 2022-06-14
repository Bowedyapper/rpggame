class Texture {
public:
	SDL_Texture* texture;
	int width;
	int height;

	Texture(SDL_Texture* tex, int w, int h) {
		texture = tex;
		width = w;
		height = h;
	}
};