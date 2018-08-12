#include "ResourceManager.h"
#include "Log.h"
#include "Config.h"
#include <SFML/Graphics/Texture.hpp>
#include "TextureAtlas.h"

//static const TexturePtr emptyTex = TexturePtr();
static const TextureRect emptyTexRect = TextureRect();

std::shared_ptr<ResourceManager>& ResourceManager::instancePtr() {
	static std::shared_ptr<ResourceManager> instancePtr {nullptr};
	return instancePtr;
}

ResourceManager* ResourceManager::Inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new ResourceManager());
	}

	return ptr.get();
}

void ResourceManager::ResetInst() {
	
	auto& ptr = instancePtr();
	if (ptr != nullptr) {
		ptr.reset();
	}
}

ResourceManager::ResourceManager() {
	Log::Inst()->PutMessage("ResourceManager::ResourceManager");
}

ResourceManager::~ResourceManager() {
	Log::Inst()->PutMessage("~ResourceManager");
}

bool ResourceManager::LoadAtlas(const std::string& tName, const std::string& descName) {

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

const SoundPtr ResourceManager::AddSound(const std::string& sName) {

	const auto& sndIt = _sounds.find(sName);

	if (sndIt != _sounds.end()) {
		const SoundPtr foundSndPtr = sndIt->second;
		Log::Inst()->PutMessage("ResourceManager::addSound: sound already added " + sName);
		return foundSndPtr;
	}

	SoundPtr sndPtr = std::make_shared<Sound>();

	const bool loadOk = sndPtr->load(Config::getResourceDir() + sName);
	if (!loadOk) {
		Log::Inst()->PutErr("ResourceManager::addSound error: unable to load " + sName);
		return SoundPtr();
	}

	_sounds[sName] = sndPtr;
	Log::Inst()->PutMessage("ResourceManager::addSound: sound added successfully: " + sName);

	return sndPtr;
}
const SoundPtr ResourceManager::GetSound(const std::string& sName, const bool onlyTry) {

	const auto& sndIt = _sounds.find(sName);

	if (sndIt == _sounds.end()) {

		if (!onlyTry) {
			Log::Inst()->PutErr("ResourceManager::getSound error, not found sound " + sName);
		}
		return SoundPtr();
	}

	return sndIt->second;
}

const TexturePtr ResourceManager::AddTexture(const std::string& tName) {

	const auto& texIt = _textures.find(tName);

	if (texIt != _textures.end()) {
		const TexturePtr foundTexPtr = texIt->second;
		Log::Inst()->PutMessage("ResourceManager::addTexture: texture already added " + tName);
		return foundTexPtr;
	}

	TexturePtr texPtr = std::make_shared<Texture>();
	const bool loadOk = texPtr->loadFromFile(Config::getResourceDir() + tName);
	if (!loadOk) {
		Log::Inst()->PutErr("ResourceManager::addTexture error: unable to load " + tName);
		return TexturePtr();
	}

	_textures[tName] = texPtr;
	Log::Inst()->PutMessage("ResourceManager::addTexture: texture added successfully: " + tName);

	return texPtr;
}

TextureRect ResourceManager::GetTexture(const std::string& tName, const bool onlyTry) {

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
					Log::Inst()->PutErr("ResourceManager::getTexture error, atlas may not be loaded yet: " + bigTexName);
					return emptyTexRect;
				}

				const TextureRect smallTextureRect{ texRectBigTex.texturePtr, smallRect };
				return smallTextureRect;
			}
		}
	}

	if (!onlyTry) {
		Log::Inst()->PutErr("ResourceManager::getTexture error, not found texture " + tName);
	}

	return emptyTexRect;
}

bool ResourceManager::RemoveTexture(const std::string& tName) {

	const auto& tIt = _textures.find(tName);
	if (tIt == _textures.end()) {
		return false;
	}
	
	_textures.erase(tIt);
	return true;
}