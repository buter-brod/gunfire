#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include "Texture.h"

class Sprite {

public:
	Sprite(const Texture& texture, const Rect& rectangle) : _sprite(*texture.getTex(), rectangle) {}
	explicit Sprite(const Texture& texture) : _sprite(*texture.getTex()) {}
	sf::Sprite* getSpr() { return &_sprite; }

private:
	sf::Sprite _sprite;
};

typedef std::shared_ptr<Sprite> SpritePtr;
