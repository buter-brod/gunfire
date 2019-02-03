#pragma once

#include "Point.h"
#include <memory>

class sfml_Game;
class Sprite;

typedef std::shared_ptr<Sprite> SpritePtr;
typedef std::shared_ptr<sfml_Game> GamePtr;

namespace sf {
	class Event;
	class RenderWindow;
}

typedef std::shared_ptr<sf::RenderWindow> WindowPtr;

class Application {
public:
	Application();
	~Application();
	void Run();

protected:
	bool init();
	void gameLoop();
	void draw();
	void freeResources();
	void startGame();
	void handleEvent(sf::Event*);

private:
	GamePtr _gamePtr;
	SpritePtr _restartBtnImg;
	SpritePtr _pauseBtnImg;
	SpritePtr _playBtnImg;
	WindowPtr _window{ nullptr };
};
