#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Log.h"

class TextureAtlas;

namespace sf {
	class Texture;
}

class Texture {

public:
	const sf::Texture* getTex() const { return &_tex; }
	bool loadFromFile(const std::string& filename, const sf::IntRect& area = sf::IntRect()) {
		
		_name = filename;
		return _tex.loadFromFile(filename, area);
	}

	~Texture() {
		Log::inst()->PutMessage("~Texture: " + _name);
	}

private:
	sf::Texture _tex;
	std::string _name;
};

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWPtr;

typedef std::shared_ptr<TextureAtlas> TextureAtlasPtr;

struct TextureRect {
	TextureWPtr texturePtr;
	sf::IntRect rect;
};

class TextureManager {
public:

	virtual ~TextureManager();

	static TextureManager* inst();
	static void resetInst();

	const TexturePtr addTexture(const std::string& tName);
	bool removeTexture(const std::string& tName);
	const TextureRect getTexture(const std::string& tName);

	bool loadAtlas(const std::string& tName, const std::string& descName);

private:
	std::unordered_map<std::string, TexturePtr> _textures;
	std::unordered_map<std::string, TextureAtlasPtr> _atlases;

	TextureManager::TextureManager();

	static std::shared_ptr<TextureManager>& instancePtr();
};