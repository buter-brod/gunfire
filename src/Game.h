#pragma once

#include "TextureManager.h"
#include <SFML/Graphics.hpp>
#include "Utils.h"

class Game {
public:
	Game();
	~Game();

	void Init();
	void Draw(sf::RenderWindow* wnd);
	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);

	Size getSize() const;

protected:
	TextureManager _textureManager;
	sf::Sprite _bgSprite;
};



