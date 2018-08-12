#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "Texture.h"
#include "Sound.h"

class TextureAtlas;
typedef std::shared_ptr<TextureAtlas> TextureAtlasPtr;

class ResourceManager {
public:

	virtual ~ResourceManager();

	static ResourceManager* Inst();
	static void ResetInst();

	const SoundPtr AddSound(const std::string& sName);
	const SoundPtr GetSound(const std::string& sName, const bool onlyTry = false);

	const TexturePtr AddTexture(const std::string& tName);
	bool RemoveTexture(const std::string& tName);
	TextureRect GetTexture(const std::string& tName, const bool onlyTry = false);

	bool LoadAtlas(const std::string& tName, const std::string& descName);

private:
	std::unordered_map<std::string, TexturePtr> _textures;
	std::unordered_map<std::string, TextureAtlasPtr> _atlases;

	std::unordered_map<std::string, SoundPtr> _sounds;

	ResourceManager::ResourceManager();

	static std::shared_ptr<ResourceManager>& instancePtr();
};