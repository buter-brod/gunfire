#pragma once

#include "Utils.h"
#include "Graphics.h"

class Game {
public:
	Game();
	~Game();

	void Init();
	void Draw(sf::RenderWindow* wnd);
	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);
	Size GetSize() const;

protected:
	std::shared_ptr<Sprite> _bgSpritePtr;
};



