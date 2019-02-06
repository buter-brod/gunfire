#pragma once
#include "sfml_Texture.h"
#include "sfml_Utils.h"

#include <SFML/Graphics/Sprite.hpp>

class Sprite {

public:
	Sprite(const sfml_Texture& texture, const Rect& r) : 
		_sprite(*texture.getTex(), Utils::toSfmlRect(r)) {}
	explicit Sprite(const sfml_Texture& texture) : _sprite(*texture.getTex()) {}
	sf::Sprite* getSpr() { return &_sprite; }

private:
	sf::Sprite _sprite;
};

typedef std::shared_ptr<Sprite> SpritePtr;
