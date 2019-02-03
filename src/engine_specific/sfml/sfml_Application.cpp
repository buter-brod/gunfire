#include "sfml_Application.h"

#include "Config.h"
#include "Log.h"

#include "sfml_Sprite.h"
#include "sfml_Game.h"
#include "sfml_Utils.h"
#include "sfml_ResourceManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

Point toGamePoint(const Point& pos) {
	const Point screenPoint(float(pos.getX()), float(pos.getY()));
	const Point gamePoint = screenPoint / CfgStatic::windowSize * CfgStatic::gameSize;
	return gamePoint;
};

Point sfToGamePoint(const sf::Vector2i& pos) {
	return toGamePoint({ (float)pos.x, (float)pos.y });
}

Application::Application() {
	Log::Inst()->PutMessage("Application::Application");
}

Application::~Application() {
	
	Log::Inst()->PutMessage("Application::~Application");
	freeResources();
	sfml_ResourceManager::ResetInst();
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

	_gamePtr = std::make_shared<sfml_Game>(_window.get());
	_gamePtr->Init();
}

bool isMouseOn(SpritePtr spr, const sf::Vector2i pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr->getSpr()->getGlobalBounds().contains(mousePosF);
	return result;	
}

void Application::handleEvent(sf::Event* event) {

	auto refreshPauseColor = [this]() {
		const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
		_gamePtr->OnCursorMoved(sfToGamePoint(mousePos));

		const bool mouseOnPauseBtn = isMouseOn(_pauseBtnImg, mousePos) || isMouseOn(_playBtnImg, mousePos);

		auto& playPauseSpr = _gamePtr->GetPaused() ? _playBtnImg : _pauseBtnImg;

		sf::Color playPauseBtnClr = mouseOnPauseBtn ? sf::Color::White : Utils::toSfmlColor(CfgStatic::btnPlayPauseNotHoveredClr);
		playPauseSpr->getSpr()->setColor(playPauseBtnClr);
	};

	switch (event->type) {
		case sf::Event::Closed: {
			_window->close();
			break;
		}

		case sf::Event::MouseMoved: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
			
			_gamePtr->OnCursorMoved(sfToGamePoint(mousePos));
			
			const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
			_restartBtnImg->getSpr()->setColor(mouseOnRestartBtn ? sf::Color::White : Utils::toSfmlColor(CfgStatic::btnPlayPauseNotHoveredClr));

			refreshPauseColor();
		}
		break;

		case sf::Event::MouseButtonPressed: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);

			const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
			if (mouseOnRestartBtn) {
				freeResources();
				startGame();
				refreshPauseColor();
				break;
			}

			const bool mouseOnPauseBtn = isMouseOn(_pauseBtnImg, mousePos);
			if (mouseOnPauseBtn) {

				const bool wasPaused = _gamePtr->GetPaused();
				_gamePtr->SetPaused(!wasPaused);
				refreshPauseColor();
				break;
			}

			_gamePtr->OnCursorClicked(sfToGamePoint(mousePos));
		}
		break;
	}
}


SpritePtr initBtn(const std::string& sprName) {
	SpritePtr spritePtr;

	const auto& texRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
	if (texRect.texturePtr.lock() == nullptr) {
		Log::Inst()->PutErr("Application::Run error, not found " + sprName);
		return spritePtr;
	}

	TexturePtr btnTex = texRect.texturePtr.lock();
	spritePtr = std::make_shared<Sprite>(*btnTex);
	spritePtr->getSpr()->setTextureRect(Utils::toSfmlRect(texRect.rect));

	return spritePtr;
}

bool Application::init() {

	const bool atlasLoadedOk = sfml_ResourceManager::Inst()->LoadAtlas(CfgStatic::atlasPng, CfgStatic::atlasMtpf);

	if (!atlasLoadedOk) {
		Log::Inst()->PutErr("Application::Run error, unable to load atlas");
		return false;
	}

	Log::Inst()->PutMessage("LOADING GAME...");

	_pauseBtnImg   = initBtn(CfgStatic::pauseImgFile);
	_playBtnImg    = initBtn(CfgStatic::playImgFile);
	_restartBtnImg = initBtn(CfgStatic::restartImgFile);

	if (_pauseBtnImg   == nullptr ||
		_playBtnImg    == nullptr ||
		_restartBtnImg == nullptr) {

		Log::Inst()->PutErr("Application::init error, buttons not initialized... ");
		return false;
	}

	_pauseBtnImg  ->getSpr()->setPosition(0.f, 0.f);
	_playBtnImg   ->getSpr()->setPosition(0.f, 0.f);
	_restartBtnImg->getSpr()->setPosition(CfgStatic::windowSize.getX() - _restartBtnImg->getSpr()->getTextureRect().width, 0.f);

	const auto& videoMode = sf::VideoMode(CfgStatic::windowSize.i_X(), CfgStatic::windowSize.i_Y());
	const auto wndFlags = sf::Style::Titlebar | sf::Style::Close;

	_window = std::make_shared<sf::RenderWindow>(videoMode, CfgStatic::appTitle, wndFlags);

	startGame();

	return true;
}

void Application::draw() {

	_gamePtr->Draw();

	auto& playPauseSpr = _gamePtr->GetPaused() ? _playBtnImg : _pauseBtnImg;
	_window->draw(*playPauseSpr->getSpr());
		
	_window->draw(*_restartBtnImg->getSpr());
	_window->display();
}

void Application::gameLoop() {

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

		const bool updated = _gamePtr->Update(frameTime);
		const bool needDraw = CfgStatic::drawFPSUnlimited || updated;

		if (needDraw) {

			draw();

			if (fpsLogFramesCount > CfgStatic::fpsLogFramesCap) { //FPS counter

				const float elapsed = Utils::dt(currentTime, fpsLogCountTimeStart);
				fpsLogCountTimeStart = currentTime;
				fpsLogFramesCount = 0;
				const auto currFPS = (unsigned int)roundf(CfgStatic::fpsLogFramesCap / elapsed);
				Log::Inst()->PutMessage("FPS: " + std::to_string(currFPS));
			}
			else {
				fpsLogFramesCount++;
			}
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