/*****************************************************************//**
 * @file   texture.h
 * @brief  This file handles texture creation, caching in VRAM and 
 * 		   freeing textures from memory, Textures are stored in a map 
 * 		   as an object containing the texture pointer, width and height
 * 
 * @author Jason Hewitt <bowedyapper@live.co.uk>
 * @date   June 2022
 *********************************************************************/
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
	std::map<std::string, TextureObject>::iterator cacheIterator;
	bool loadTexture(const char *name, const char *filePath);
	void assignRenderer(SDL_Renderer *rendererPtr);
	TextureObject getTexture(const char *textureName);std::string getSystemTime();
	bool unloadTexture(std::string textureName);
	SDL_Renderer* renderer;
private:
	SDL_Surface* loadImg(const char* filePath);
	TextureObject createTextureFromSurface(SDL_Surface* surface);
};


SDL_Surface* TextureCache::loadImg(const char *filePath) {
	SDL_Surface* surface = IMG_Load(filePath);
	//utils::debugLog("info", "Trying to load image from: " + (std::string)filePath);
	return surface;
}

TextureObject TextureCache::createTextureFromSurface(SDL_Surface* surface) {
	SDL_Texture* renderedTexture = SDL_CreateTextureFromSurface(renderer, surface);
	TextureObject texture(renderedTexture, surface->w, surface->h);
	SDL_FreeSurface(surface);
	return texture;
}

bool TextureCache::loadTexture(const char* name, const char *filePath) {
	SDL_Surface* surface = loadImg(filePath);
	if (!surface) {
		utils::debugLog("error", "Could not load image from: " + (std::string)filePath);
		return false;
	}
	//utils::debugLog("info", "Image loaded successfully from: " + (std::string)filePath);


	TextureObject texture = createTextureFromSurface(surface);

	if (!texture.width) {
		utils::debugLog("error", "Something went wrong converting surface from: " + (std::string)filePath);
		return false;
	}
	else {
		//utils::debugLog("info", "Surface created successfully from: " + (std::string)filePath);
		
		if (cache.find(name) != cache.end()) {
			utils::debugLog("error", "Texture could not be loaded into VRAM with name " + (std::string)name + " because it already exists");
			SDL_DestroyTexture(texture.texture);
		}
		else {
			cache.insert(textureKeyPair(name, texture));
			utils::debugLog("info", "Texture loaded into memory successfully as " + (std::string)name + " from " + (std::string)filePath);
		}

		return true;
	}
	
}

void TextureCache::assignRenderer(SDL_Renderer* rendererPtr) {
	renderer = rendererPtr;
}

TextureObject TextureCache::getTexture(const char *textureName) {
	
	if (cache.find(textureName) == cache.end()) {
		utils::debugLog("error", "Could not find texture " + (std::string)textureName + " in the cache");
		return TextureObject(NULL, 1, 1);
	}
	else {
		utils::debugLog("info", "Found texture " + (std::string)textureName + " in the cache");
		auto getTextureFromMap = cache.at(textureName);
		return getTextureFromMap;
	}
}

bool TextureCache::unloadTexture(std::string textureName) {
	if (cache.find(textureName) == cache.end()) {
		utils::debugLog("error", "Could not unload texture " + (std::string)textureName + " from the cache");
		return false;
	}
	else {
		utils::debugLog("info", "Unloading texture " + (std::string)textureName + " from the cache");
		SDL_DestroyTexture(cache.at(textureName).texture);
		cache.erase(textureName);
		return true;
	}
}