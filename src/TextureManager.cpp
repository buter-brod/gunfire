#include "TextureManager.h"
#include "Log.h"
#include "Config.h"
#include <SFML/Graphics/Texture.hpp>
#include "TextureAtlas.h"

static const TexturePtr emptyTex = TexturePtr();
static const TextureRect emptyTexRect = TextureRect();

std::shared_ptr<TextureManager>& TextureManager::instancePtr() {
	static std::shared_ptr<TextureManager> instancePtr {nullptr};
	return instancePtr;
}

TextureManager* TextureManager::inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new TextureManager());
	}

	return ptr.get();
}

void TextureManager::resetInst() {
	
	auto& ptr = instancePtr();
	if (ptr != nullptr) {
		ptr.reset();
	}
}

TextureManager::TextureManager() {
	Log::inst()->PutMessage("TextureManager::TextureManager");
}

TextureManager::~TextureManager() {
	Log::inst()->PutMessage("~TextureManager");
}

bool TextureManager::loadAtlas(const std::string& tName, const std::string& descName) {

	TextureAtlasPtr atlasPtr = std::make_shared<TextureAtlas>();
	const bool loaded = atlasPtr->load(descName, tName);

	if (!loaded) {
		return false;
	}

	const TexturePtr& atlasTexPtr = addTexture(tName);

	if (atlasTexPtr == nullptr) {
		return false;
	}

	_atlases[tName] = atlasPtr;

	return true;
}

const TexturePtr TextureManager::addTexture(const std::string& tName) {

	const auto& texIt = _textures.find(tName);

	if (texIt != _textures.end()) {
		const TexturePtr foundTexPtr = texIt->second;
		Log::inst()->PutMessage("TextureManager::addTexture: texture already added " + tName);
		return foundTexPtr;
	}

	TexturePtr texPtr = std::make_shared<Texture>();
	const bool loadOk = texPtr->loadFromFile(Config::getResourceDir() + tName);
	if (!loadOk) {
		Log::inst()->PutErr("TextureManager::addTexture error: unable to load " + tName);
		return emptyTex;
	}

	_textures[tName] = texPtr;
	Log::inst()->PutMessage("TextureManager::addTexture: texture added successfully: " + tName);

	return texPtr;
}

const TextureRect TextureManager::getTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);

	if (tIt != _textures.end()) {
		const TexturePtr& ptr = tIt->second;
		const auto tSize = ptr->getTex()->getSize();

		const sf::IntRect fullTexRect(0, 0, tSize.x, tSize.y);
		const TextureRect textureRect { ptr, fullTexRect };

		return textureRect;

	} else { // not a standalone texture, will look in atlases

		for (const auto& atlIt : _atlases) {
			const TextureAtlas& atlas = *atlIt.second;
			const auto& smallRect = atlas.getRect(tName);

			if (smallRect != sf::IntRect()) { // found!

				const std::string& bigTexName = atlas.getTextureName();
				const auto& texRectBigTex = getTexture(bigTexName);

				if (texRectBigTex.texturePtr.lock() == nullptr) {
					Log::inst()->PutErr("TextureManager::getTexture error, atlas may not be loaded yet: " + bigTexName);
					return emptyTexRect;
				}

				const TextureRect smallTextureRect{ texRectBigTex.texturePtr, smallRect };
				return smallTextureRect;
			}
		}
	}

	Log::inst()->PutErr("TextureManager::getTexture error, not found texture " + tName);
	return emptyTexRect;
}

bool TextureManager::removeTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {
		return false;
	}
	
	_textures.erase(tIt);
	return true;
}