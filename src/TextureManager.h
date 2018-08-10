#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Log.h"
#include "Graphics.h"

typedef std::shared_ptr<TextureAtlas> TextureAtlasPtr;

class TextureManager {
public:

	virtual ~TextureManager();

	static TextureManager* Inst();
	static void ResetInst();

	const TexturePtr AddTexture(const std::string& tName);
	bool RemoveTexture(const std::string& tName);
	const TextureRect GetTexture(const std::string& tName);

	bool LoadAtlas(const std::string& tName, const std::string& descName);

private:
	std::unordered_map<std::string, TexturePtr> _textures;
	std::unordered_map<std::string, TextureAtlasPtr> _atlases;

	TextureManager::TextureManager();

	static std::shared_ptr<TextureManager>& instancePtr();
};