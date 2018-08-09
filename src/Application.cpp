#include "Application.h"
#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Game.h"

Application::Application() {

}

Application::~Application() {

}

void Application::restart() {

	_gamePtr = std::make_shared<Game>();
}

void Application::Run() {

	restart();

	sf::RenderWindow window(sf::VideoMode(Config::screenW, Config::screenH), "gunfire");

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::Black);
		_gamePtr->Draw(&window);
		window.display();
	}
}