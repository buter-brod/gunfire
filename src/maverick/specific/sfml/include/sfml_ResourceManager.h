#pragma once
#include "sfml_Texture.h"

#include <unordered_map>
#include <string>
#include <memory>

class TextureAtlas;
typedef std::shared_ptr<TextureAtlas> TextureAtlasPtr;

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;

class sfml_ResourceManager {
public:

	virtual ~sfml_ResourceManager();

	static sfml_ResourceManager* Inst();
	static void ResetInst();

	const ShaderPtr AddShader(const std::string& sName);
	const ShaderPtr GetShader(const std::string& sName, const bool onlyTry = false);

	const TexturePtr AddTexture(const std::string& tName);
	bool RemoveTexture(const std::string& tName);
	TextureRect GetTexture(const std::string& tName, const bool onlyTry = false);

	bool LoadAtlas(const std::string& tName, const std::string& descName);

private:
	sfml_ResourceManager::sfml_ResourceManager();
	static std::shared_ptr<sfml_ResourceManager>& instancePtr();

	template<class ResType> using ResMap = std::unordered_map<std::string, ResType>;

	ResMap<TexturePtr> _textures;
	ResMap<TextureAtlasPtr> _atlases;
	ResMap<ShaderPtr> _shaders;
};
