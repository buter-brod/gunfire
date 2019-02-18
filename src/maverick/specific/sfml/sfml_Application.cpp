#include "sfml_Application.h"

#include "CFCfgStatic.h"
#include "Config.h"
#include "Log.h"

#include "sfml_Sprite.h"
#include "sfml_Game.h"
#include "sfml_ResourceManager.h"
#include "sfml_SoundManager.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

static bool isMouseOn(const SpritePtr& spr, const sf::Vector2i& pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr->getSpr()->getGlobalBounds().contains(mousePosF);
	return result;
}

Point toGamePoint(const Point& pos) {
	const Point screenPoint(float(pos.getX()), float(pos.getY()));
	const Point gamePoint = screenPoint / CfgStatic::windowSize * CfgStatic::gameSize;
	return gamePoint;
}

static Point sfToGamePoint(const sf::Vector2i& pos) {
	return toGamePoint({ (float)pos.x, (float)pos.y });
}

sfml_Application::sfml_Application() {
	Log::Inst()->PutMessage("Application::Application");
}

sfml_Application::~sfml_Application() {

	Log::Inst()->PutMessage("Application::~Application");
	freeResources();
	sfml_ResourceManager::ResetInst();
}

void sfml_Application::freeResources() {

	Log::Inst()->PutMessage("Application::freeResources");
	_gamePtr = nullptr;
}

void sfml_Application::startGame(const GameInitParams& params = {}) {

	Log::Inst()->PutMessage("Application::startGame");

	if (_gamePtr) {
		Log::Inst()->PutErr("Application::startGame error, game is running already");
		return;
	}

	sfml_SoundManager::Create();

	_gamePtr = createGame(params);
	_gamePtr->Init();
}

bool sfml_Application::onMouseMoved(const int x, const int y) { return false; }
bool sfml_Application::onMousePressed(const int x, const int y) { return false; }

void sfml_Application::handleEvent(sf::Event* event) {

	switch (event->type) {
		case sf::Event::Closed: {
			_window->close();
			break;
		}

		case sf::Event::MouseMoved: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);
			_gamePtr->OnCursorMoved(sfToGamePoint(mousePos));
			onMouseMoved(mousePos.x, mousePos.y);
			break;
		}

		case sf::Event::MouseButtonPressed: {
			const sf::Vector2i mousePos = sf::Mouse::getPosition(*_window);

			const bool handledByApp = onMousePressed(mousePos.x, mousePos.y);

			if (!handledByApp) {
				_gamePtr->OnCursorClicked(sfToGamePoint(mousePos));
			}
			
			break;
		}
		default: {
		}
	}
}

SpritePtr sfml_Application::initBtn(const std::string& sprName) const {
	SpritePtr spritePtr;

	const auto& texRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
	if (texRect.texturePtr.lock() == nullptr) {
		Log::Inst()->PutErr("Application::Run error, not found " + sprName);
		return spritePtr;
	}

	const TexturePtr btnTex = texRect.texturePtr.lock();
	spritePtr = std::make_shared<Sprite>(*btnTex);
	spritePtr->getSpr()->setTextureRect(Utils::toSfmlRect(texRect.rect));

	return spritePtr;
}

bool sfml_Application::init() {

	const bool atlasLoadedOk = sfml_ResourceManager::Inst()->LoadAtlas(CfgStatic::atlasPng, CfgStatic::atlasMtpf);

	if (!atlasLoadedOk) {
		Log::Inst()->PutErr("sfml_Application::Run error, unable to load atlas");
		return false;
	}

	Log::Inst()->PutMessage("LOADING GAME...");

	const auto& videoMode = sf::VideoMode(CfgStatic::windowSize.i_X(), CfgStatic::windowSize.i_Y());
	const auto wndFlags = sf::Style::Titlebar | sf::Style::Close;

	initSpecial();

	_window = std::make_shared<sf::RenderWindow>(videoMode, CfgStatic::appTitle, wndFlags);
	startGame();

	return true;
}

void sfml_Application::drawSpecial() {}

bool sfml_Application::initSpecial() {
	return true;
}

void sfml_Application::draw() {

	_gamePtr->Draw();
	drawSpecial();
	_window->display();
}

void sfml_Application::gameLoop() {

	time_us currentTime = Utils::getTime();
	unsigned int fpsLogFramesCount = 0;
	time_us fpsLogCountTimeStart = currentTime;

	const bool fpsUnlimited = Config::Inst()->getInt("drawFPSUnlimited") > 0;

	while (_window->isOpen()) {

		const time_us newTime = Utils::getTime();
		const float frameTime = Utils::dt(newTime, currentTime);
		currentTime = newTime;

		const bool updated = _gamePtr->Update(frameTime);
		const bool needDraw = fpsUnlimited || updated;

		if (updated){
			sf::Event event;
			while (_window->pollEvent(event)) {
				handleEvent(&event);
			}			
		}

		if (needDraw) {
			draw();

			const float elapsed = Utils::dt(currentTime, fpsLogCountTimeStart);

			if (elapsed > CfgStatic::fpsLogTime) { //FPS counter
				
				const auto currFPS = (unsigned int)roundf(fpsLogFramesCount / elapsed);
				Log::Inst()->PutMessage("FPS: " + std::to_string(currFPS));

				fpsLogCountTimeStart = currentTime;
				fpsLogFramesCount = 0;
			}
			else {
				fpsLogFramesCount++;
			}
		}
	}
}

void sfml_Application::Run() {

	const bool initOk = init();

	if (!initOk) {
		return;
	}

	gameLoop();
}