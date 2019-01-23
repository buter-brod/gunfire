#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include "Texture.h"
#include "sfml_Utils.h"

class Sprite {

public:
	Sprite(const Texture& texture, const Utils::Rect& r) : 
		_sprite(*texture.getTex(), Utils::toSfmlRect(r)) {}
	explicit Sprite(const Texture& texture) : _sprite(*texture.getTex()) {}
	sf::Sprite* getSpr() { return &_sprite; }

private:
	sf::Sprite _sprite;
};

typedef std::shared_ptr<Sprite> SpritePtr;
