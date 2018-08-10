#pragma once

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
	sf::Sprite _bgSprite;
};



