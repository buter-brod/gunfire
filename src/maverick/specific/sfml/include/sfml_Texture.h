#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "Rect.h"

class sfml_Texture {

public:
	sfml_Texture() = default;
	const sf::Texture* getTex() const;
	bool loadFromFile(const std::string& filename, const Rect& area = Rect());
	~sfml_Texture();

private:
	sf::Texture _tex;
	std::string _name;
};

typedef std::shared_ptr<sfml_Texture> TexturePtr;
typedef std::weak_ptr<sfml_Texture> TextureWPtr;

struct TextureRect {
	TextureWPtr texturePtr;
	Rect rect;

	bool isEmpty() const {
		return
			rect._size.getX() == 0.f &&
			rect._size.getY() == 0.f;
	}
};