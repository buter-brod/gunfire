#include "Application.h"
#include "Config.h"
#include "Game.h"
#include "Log.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

Application::Application() {
	Log::Inst()->PutMessage("Application::Application");
}

Application::~Application() {
	
	Log::Inst()->PutMessage("Application::~Application");
	freeResources();
	ResourceManager::ResetInst();
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
	_gamePtr->Update(0.f);
}

bool isMouseOn(SpritePtr spr, const sf::Vector2i pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr->getSpr()->getGlobalBounds().contains(mousePosF);
	return result;	
}

void Application::handleEvent(sf::Event* event) {
	switch (event->type) {
		case sf::Event::Closed: {
			_window->close();
			break;
		}

		case sf::Event::MouseMoved: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
			
			_gamePtr->OnCursorMoved(toGamePoint(mousePos));
			
			const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
			_restartBtnImg->getSpr()->setColor(mouseOnRestartBtn ? sf::Color::White : CfgStatic::btnNotHovered);
		}
		break;

		case sf::Event::MouseButtonPressed: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);

			_gamePtr->OnCursorClicked(toGamePoint(mousePos));

			const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
			if (mouseOnRestartBtn) {
				freeResources();
				startGame();
			}
		}
		break;
	}
}

Point Application::toGamePoint(const sf::Vector2i pos){
	const Point screenPoint = Point(float(pos.x), float(pos.y));
	const Point gamePoint = screenPoint / CfgStatic::windowSize * _gamePtr->GetSize();
	return gamePoint;
};

bool Application::init() {

	const bool atlasLoadedOk = ResourceManager::Inst()->LoadAtlas(CfgStatic::atlasPng, CfgStatic::atlasMtpf);

	if (!atlasLoadedOk) {
		Log::Inst()->PutErr("Application::Run error, unable to load atlas");
		return false;
	}

	Log::Inst()->PutMessage("LOADING GAME...");

	const auto& texRect = ResourceManager::Inst()->GetTexture(CfgStatic::restartImgFile);
	if (texRect.texturePtr.lock() == nullptr) {
		Log::Inst()->PutErr("Application::Run error, not found " + CfgStatic::restartImgFile);
		return false;
	}

	TexturePtr restartBtnTex = texRect.texturePtr.lock();
	_restartBtnImg = std::make_shared<Sprite>(*restartBtnTex);
	_restartBtnImg->getSpr()->setTextureRect(texRect.rect);
	_restartBtnImg->getSpr()->setPosition(CfgStatic::windowSize.getX() - _restartBtnImg->getSpr()->getTextureRect().width, 0.f);

	startGame();

	_window = std::make_shared<sf::RenderWindow>(sf::VideoMode(CfgStatic::windowSize.i_X(), CfgStatic::windowSize.i_Y()), CfgStatic::appTitle, sf::Style::Titlebar | sf::Style::Close);

	return true;
}

void Application::draw() {

	_gamePtr->Draw(_window.get());
	_window->draw(*_restartBtnImg->getSpr());
	_window->display();
}

void Application::gameLoop() {

	const float dt = 1.f / CfgStatic::simulationFPS;
	float timeAccumulator = 0.f;
	time_us currentTime = Utils::getTime();
	unsigned int fpsLogFramesCount = 0;
	time_us fpsLogCountTimeStart = currentTime;

	while (_window->isOpen()) {
		sf::Event event;
		while (_window->pollEvent(event)) {
			handleEvent(&event);
		}

		const time_us newTime = Utils::getTime();
		const float frameTime = Utils::dt(newTime, currentTime);
		currentTime = newTime;
		timeAccumulator += frameTime;

		while (timeAccumulator >= dt) {
			_gamePtr->Update(dt);
			timeAccumulator -= dt;
		}

		draw();

		if (fpsLogFramesCount > CfgStatic::fpsLogFramesCap) { //FPS counter

			const float elapsed = Utils::dt(currentTime, fpsLogCountTimeStart);
			fpsLogCountTimeStart = currentTime;
			fpsLogFramesCount = 0;
			const auto currFPS = CfgStatic::fpsLogFramesCap / elapsed;
			Log::Inst()->PutMessage("FPS: " + std::to_string(int(currFPS)));
		}
		else {
			fpsLogFramesCount++;
		}
	}
}

void Application::Run() {
	
	const bool initOk = init();

	if (!initOk) {
		return;
	}
	
	gameLoop();
}