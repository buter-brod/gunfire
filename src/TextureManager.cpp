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

TextureManager* TextureManager::Inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new TextureManager());
	}

	return ptr.get();
}

void TextureManager::ResetInst() {
	
	auto& ptr = instancePtr();
	if (ptr != nullptr) {
		ptr.reset();
	}
}

TextureManager::TextureManager() {
	Log::Inst()->PutMessage("TextureManager::TextureManager");
}

TextureManager::~TextureManager() {
	Log::Inst()->PutMessage("~TextureManager");
}

bool TextureManager::LoadAtlas(const std::string& tName, const std::string& descName) {

	TextureAtlasPtr atlasPtr = std::make_shared<TextureAtlas>();
	const bool loaded = atlasPtr->load(descName, tName);

	if (!loaded) {
		return false;
	}

	const TexturePtr& atlasTexPtr = AddTexture(tName);

	if (atlasTexPtr == nullptr) {
		return false;
	}

	_atlases[tName] = atlasPtr;

	return true;
}

const TexturePtr TextureManager::AddTexture(const std::string& tName) {

	const auto& texIt = _textures.find(tName);

	if (texIt != _textures.end()) {
		const TexturePtr foundTexPtr = texIt->second;
		Log::Inst()->PutMessage("TextureManager::addTexture: texture already added " + tName);
		return foundTexPtr;
	}

	TexturePtr texPtr = std::make_shared<Texture>();
	const bool loadOk = texPtr->loadFromFile(Config::getResourceDir() + tName);
	if (!loadOk) {
		Log::Inst()->PutErr("TextureManager::addTexture error: unable to load " + tName);
		return emptyTex;
	}

	_textures[tName] = texPtr;
	Log::Inst()->PutMessage("TextureManager::addTexture: texture added successfully: " + tName);

	return texPtr;
}

TextureRect TextureManager::GetTexture(const std::string& tName, const bool onlyTry) {

	const auto& tIt = _textures.find(tName);

	if (tIt != _textures.end()) {
		const TexturePtr& ptr = tIt->second;
		const auto tSize = ptr->getTex()->getSize();

		const Rect fullTexRect(0, 0, tSize.x, tSize.y);
		const TextureRect textureRect { ptr, fullTexRect };

		return textureRect;

	} else { // not a standalone texture, will look in atlases

		for (const auto& atlIt : _atlases) {
			const TextureAtlas& atlas = *atlIt.second;
			const auto& smallRect = atlas.getRect(tName);

			if (smallRect != Rect()) { // found!

				const std::string& bigTexName = atlas.getTextureName();
				const auto& texRectBigTex = GetTexture(bigTexName);

				if (texRectBigTex.texturePtr.lock() == nullptr) {
					Log::Inst()->PutErr("TextureManager::getTexture error, atlas may not be loaded yet: " + bigTexName);
					return emptyTexRect;
				}

				const TextureRect smallTextureRect{ texRectBigTex.texturePtr, smallRect };
				return smallTextureRect;
			}
		}
	}

	if (!onlyTry) {
		Log::Inst()->PutErr("TextureManager::getTexture error, not found texture " + tName);
	}

	return emptyTexRect;
}

bool TextureManager::RemoveTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {
		return false;
	}
	
	_textures.erase(tIt);
	return true;
}