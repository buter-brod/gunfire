#pragma once
#include "Utils.h"
#include <map>
#include <time.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include "GameObject.h"
#include <time.h>

class Sprite;

class Game {
public:
	Game();
	~Game();

	void Init();
	void Draw(sf::RenderWindow* wnd);
	void Update();

	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);
	
	Size GetSize() const;

	IDType Game::newID();

protected:
	std::shared_ptr<Sprite> _bgSpritePtr;
	IDType _nextID{ 1 };
	clock_t _clock{ 0 };

	std::map<int, GameObjectPtr> _objects;
};



