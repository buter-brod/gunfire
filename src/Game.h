#pragma once

#include "TextureManager.h"
#include <SFML/Graphics.hpp>

class Game {
public:
	Game();
	~Game();

	void Init();
	void Draw(sf::RenderWindow* wnd);

protected:
	TextureManager _textureManager;
	sf::Sprite _bgSprite;
};



