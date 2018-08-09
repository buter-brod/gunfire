#pragma once

class Game;
#include <memory>

class Application {
public:
	Application();
	~Application();

	void Run();

protected:
	void startGame();

private:

	std::shared_ptr<Game> _gamePtr;
};
