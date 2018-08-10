#include "Application.h"
#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Game.h"
#include "Log.h"
#include "TextureManager.h"

Application::Application() {
	Log::Inst()->PutMessage("Application::Application");
}

Application::~Application() {
	
	Log::Inst()->PutMessage("Application::~Application");
	freeResources();
	TextureManager::ResetInst();
}

void Application::freeResources() {

	Log::Inst()->PutMessage("Application::freeResources");
	_gamePtr = nullptr;
}

void Application::startGame() {

	Log::Inst()->PutMessage("Application::startGame");

	if (_gamePtr) {
		Log::Inst()->PutErr("Application::startGame error, game is running already");
		return;
	}

	_gamePtr = std::make_shared<Game>();
}

bool isMouseOn(const sf::Sprite& spr, const sf::Vector2i pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr.getGlobalBounds().contains(mousePosF);
	return result;	
}

void Application::Run() {
		
	const bool atlasLoadedOk = TextureManager::Inst()->LoadAtlas("atlas.png", "atlas.mtpf");

	if (!atlasLoadedOk) {
		Log::Inst()->PutErr("Application::Run error, unable to load atlas");
		return;
	}

	startGame();

	const auto& texRect = TextureManager::Inst()->GetTexture(Config::restartImgFile);
	if (texRect.texturePtr.lock() == nullptr) {
		Log::Inst()->PutErr("Application::Run error, not found " + Config::restartImgFile);
		return;
	}

	const sf::Texture& restartBtnTex = *texRect.texturePtr.lock()->getTex();
	sf::Sprite restartBtnImg;

	restartBtnImg.setTexture(restartBtnTex);
	restartBtnImg.setTextureRect(texRect.rect);

	restartBtnImg.setPosition(Config::windowSize.getX() - restartBtnImg.getTextureRect().width, 0.f);

	sf::RenderWindow window(sf::VideoMode(Config::windowSize.i_X(), Config::windowSize.i_Y()), Config::appTitle);

	auto toGamePoint = [&window, this](const sf::Vector2i pos) -> Point {
		const Point screenPoint = Point(float(pos.x), float(pos.y));
		const Point gamePoint = screenPoint / Config::windowSize * _gamePtr->GetSize();
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
						freeResources();
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