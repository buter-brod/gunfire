#pragma once

#include <string>
#include <SFML/Graphics/Texture.hpp>
#include "Utils.h"

class Texture {

public:
	const sf::Texture* getTex() const;
	bool loadFromFile(const std::string& filename, const Utils::Rect& area = Utils::Rect());
	~Texture();

private:
	sf::Texture _tex;
	std::string _name;
};

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWPtr;

struct TextureRect {
	TextureWPtr texturePtr;
	Utils::Rect rect;

	bool isEmpty() const {
		return
			rect._size.getX() == 0.f &&
			rect._size.getY() == 0.f;
	}
};