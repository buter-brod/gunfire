#pragma once

#include "MiscForward.h"
#include "Point.h"

class Sprite;
typedef std::shared_ptr<Sprite> SpritePtr;

namespace sf {
	class Event;
	class RenderWindow;
}

typedef std::shared_ptr<sf::RenderWindow> WindowPtr;

typedef std::map<std::string, std::string> GameInitParams;

Point toGamePoint(const Point& pos);

class sfml_Application {
public:
	sfml_Application();
	~sfml_Application();
	void Run();

protected:
	bool init();
	virtual bool initSpecial();
	virtual void drawSpecial();
	virtual GamePtr createGame(const GameInitParams params) = 0;

	void gameLoop();
	void draw();
	void freeResources();
	void startGame(const GameInitParams params);

	GamePtr getGamePtr() const { return _gamePtr; }
	WindowPtr getWindowPtr() const { return _window; }

	SpritePtr initBtn(const std::string& sprName) const;

	virtual void handleEvent(sf::Event*);

	virtual bool onMouseMoved(const int x, const int y);
	virtual bool onMousePressed(const int x, const int y);

	static Point toGamePoint(const Point& pos);

private:
	GamePtr _gamePtr;
	WindowPtr _window{ nullptr };
};
