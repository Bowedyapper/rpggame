class TextureObject {
public:
	SDL_Texture* texture;
	int width;
	int height;

	TextureObject(SDL_Texture* tex, int w, int h) {
		texture = tex;
		width = w;
		height = h;
	}
};

typedef std::pair<std::string, TextureObject> textureKeyPair;

class TextureCache{
public:
	std::map<std::string, TextureObject> cache;
	bool loadTexture(char* name, char* filePath);
	void assignRenderer(SDL_Renderer* rendererPtr);
	TextureObject getTexture(std::string textureName);
	SDL_Renderer* renderer;
private:
	SDL_Surface* loadImg(char* filePath);
	TextureObject createTextureFromSurface(SDL_Surface* surface);
};



SDL_Surface* TextureCache::loadImg(char* filePath) {
	SDL_Surface* surface = IMG_Load(filePath);
	std::cout << "Trying to load image " << filePath << std::endl;
	return surface;
}

TextureObject TextureCache::createTextureFromSurface(SDL_Surface* surface) {
	SDL_Texture* renderedTexture = SDL_CreateTextureFromSurface(renderer, surface);
	TextureObject texture(renderedTexture, surface->w, surface->h);
	SDL_FreeSurface(surface);
	return texture;
}

bool TextureCache::loadTexture(char* name, char* filePath) {
	SDL_Surface* surface = loadImg(filePath);
	if (!surface) {
		std::cerr << "Could not load image at " << filePath << std::endl;
		return false;
	}

	std::cout << "Image loaded successfully from " << filePath << std::endl;

	TextureObject texture = createTextureFromSurface(surface);

	if (!texture.width) {
		std::cerr << "Something went wrong converting surface " << filePath << " to a texture" << std::endl;
		return false;
	}
	else {
		std::cout << "Surface created successfully from " << filePath << std::endl;
		cache.insert(textureKeyPair(name, texture));
		if (cache.find(name) == cache.end()) {
			std::cerr << "Texture could not be loaded into memory" << std::endl;
		}
		else {
			std::cout << "Texture loaded into memory successfully as " << name << std::endl;
		}

		return true;
	}
	
}

void TextureCache::assignRenderer(SDL_Renderer* rendererPtr) {
	renderer = rendererPtr;
}

TextureObject TextureCache::getTexture(std::string textureName) {
	
	if (cache.find(textureName) == cache.end()) {
		std::cerr << "Could not find texture \"" << textureName << "\" in textureCache" << std::endl;
	}
	else {
		std::cout << "Found texture with name \"" << textureName << "\"" << std::endl;
		auto getTextureFromMap = cache.at(textureName);
		return getTextureFromMap;
	}
}