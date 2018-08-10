#pragma once
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class TextureAtlas {
public:
	bool load(const std::string& fileName, const std::string& texName);
	const sf::IntRect& getRect(const std::string &name) const;
	const std::string& getTextureName() const;

private:
	std::string _textureName;
	std::unordered_map<std::string, sf::IntRect> _rects;
};
