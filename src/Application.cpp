#include "Application.h"
#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Game.h"
#include "Log.h"

Application::Application() {
}

Application::~Application() {
}

void Application::startGame() {
	_gamePtr = nullptr;
	_gamePtr = std::make_shared<Game>();
}

bool isMouseOn(const sf::Sprite& spr, const sf::Vector2i pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr.getGlobalBounds().contains(mousePosF);
	return result;	
}

void Application::Run() {
		
	startGame();
	sf::Texture restartBtnTex;	
	sf::Sprite restartBtnImg;

	if (!restartBtnTex.loadFromFile(Config::restartImgFile)) {
		Log::inst()->PutErr("Application::Run error, cannot load " + Config::restartImgFile);
		return;
	}		

	restartBtnImg.setTexture(restartBtnTex);
	restartBtnImg.setPosition(Config::windowSize.getX() - restartBtnTex.getSize().x, 0.f);

	sf::RenderWindow window(sf::VideoMode(Config::windowSize.i_X(), Config::windowSize.i_Y()), Config::appTitle);

	auto toGamePoint = [&window, this](const sf::Vector2i pos) -> Point {
		const Point screenPoint = Point(float(pos.x), float(pos.y));
		const Point gamePoint = screenPoint / Config::windowSize * _gamePtr->getSize();
		return gamePoint;
	};

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {

			switch (event.type) {
				case sf::Event::Closed: {
					window.close();
					break;
				}

				case sf::Event::MouseMoved:	{
					const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

					_gamePtr->OnCursorMoved(toGamePoint(mousePos));

					const bool mouseOnRestartBtn = isMouseOn(restartBtnImg, mousePos);
					restartBtnImg.setColor(mouseOnRestartBtn ? sf::Color::White : Config::btnNotHovered);
				}
				break;

				case sf::Event::MouseButtonPressed:	{
					const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
										
					_gamePtr->OnCursorClicked(toGamePoint(mousePos));

					const bool mouseOnRestartBtn = isMouseOn(restartBtnImg, mousePos);
					if (mouseOnRestartBtn) {
						startGame();
					}
				}
				break;
			}
		}

		window.clear(sf::Color::Black);
		_gamePtr->Draw(&window);

		window.draw(restartBtnImg);

		window.display();
	}
}