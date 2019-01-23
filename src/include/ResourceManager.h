#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "Texture.h"
#include "Shader.h"
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

	const ShaderPtr AddShader(const std::string& sName);
	const ShaderPtr GetShader(const std::string& sName, const bool onlyTry = false);

	const TexturePtr AddTexture(const std::string& tName);
	bool RemoveTexture(const std::string& tName);
	TextureRect GetTexture(const std::string& tName, const bool onlyTry = false);

	bool LoadAtlas(const std::string& tName, const std::string& descName);

private:

	ResourceManager::ResourceManager();
	static std::shared_ptr<ResourceManager>& instancePtr();

	template<class ResType> using ResMap = std::unordered_map<std::string, ResType>;

	ResMap<TexturePtr> _textures;
	ResMap<TextureAtlasPtr> _atlases;
	ResMap<ShaderPtr> _shaders;
	ResMap<SoundPtr> _sounds;
};