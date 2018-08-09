#include "TextureManager.h"
#include "Log.h"
#include "Config.h"
#include <SFML/Graphics/Texture.hpp>

static const TexturePtr emptyTex = TexturePtr();

TextureManager::TextureManager() {
	Log::inst()->PutMessage("TextureManager");
}

TextureManager::~TextureManager() {
	Log::inst()->PutMessage("~TextureManager");
}

const TexturePtr TextureManager::addTexture(const std::string& tName) {

	if (_textures.count(tName) > 0) {

		Log::inst()->PutErr("TextureManager::addTexture error, texture already loaded " + tName);
		return emptyTex;
	}

	TexturePtr texPtr = std::make_shared<Texture>();
	const bool loadOk = texPtr->loadFromFile(Config::getResourceDir() + tName);
	if (!loadOk) {
		Log::inst()->PutErr("TextureManager::addTexture error: unable to load " + tName);
		return emptyTex;
	}

	_textures[tName] = texPtr;
	return texPtr;
}
const TexturePtr TextureManager::getTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {

		Log::inst()->PutErr("TextureManager::getTexture error, not found texture " + tName);
		return emptyTex;
	}

	const TexturePtr& ptr = tIt->second;
	return ptr;
}


bool TextureManager::removeTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {
		return false;
	}
	
	_textures.erase(tIt);
	return true;
}