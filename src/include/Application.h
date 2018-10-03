#pragma once

class Game;
#include <memory>
#include "Sprite.h"
#include "Utils.h"

namespace sf {
	class Event;
	class RenderWindow;
}

typedef std::shared_ptr<sf::RenderWindow> WindowPtr;

class Application {
public:
	Application();
	~Application();

	bool init();
	void gameLoop();
	void draw();
	void Run();

protected:
	void freeResources();
	void startGame();
	void handleEvent(sf::Event*);
	Point toGamePoint(const sf::Vector2i pos);

private:

	std::shared_ptr<Game> _gamePtr;
	SpritePtr _restartBtnImg;
	WindowPtr _window{ nullptr };
};
