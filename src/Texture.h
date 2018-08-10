#pragma once

#include <string>
#include <SFML/Graphics/Texture.hpp>
#include "Rect.h"

class Texture {

public:
	const sf::Texture* getTex() const;
	bool loadFromFile(const std::string& filename, const Rect& area = Rect());
	~Texture();

private:
	sf::Texture _tex;
	std::string _name;
};

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWPtr;

struct TextureRect {
	TextureWPtr texturePtr;
	Rect rect;

	bool isEmpty() const { return rect.width == 0; }
};