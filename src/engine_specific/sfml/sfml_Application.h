#pragma once

#include <memory>
#include "Sprite.h"
#include "Utils.h"

class Game;
typedef std::shared_ptr<Game> GamePtr;

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

	GamePtr _gamePtr;
	SpritePtr _restartBtnImg;
	SpritePtr _pauseBtnImg;
	WindowPtr _window{ nullptr };
};
