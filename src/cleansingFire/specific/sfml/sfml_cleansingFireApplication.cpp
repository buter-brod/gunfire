#include "sfml_cleansingFireApplication.h"
#include "sfml_cleansingFireGame.h"

#include "CFCfgStatic.h"
#include "Log.h"

#include "sfml_Sprite.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

static const std::string skipIntroParam = "skipIntro";

sfml_cleansingFireApplication::sfml_cleansingFireApplication() {
	Log::Inst()->PutMessage("sfml_cleansingFireApplication::sfml_cleansingFireApplication");
}

sfml_cleansingFireApplication::~sfml_cleansingFireApplication() {
	Log::Inst()->PutMessage("sfml_cleansingFireApplication::~sfml_cleansingFireApplication");
}

static Point sfToGamePoint(const sf::Vector2i& pos) {
	return toGamePoint({ (float)pos.x, (float)pos.y });
}

static bool isMouseOn(SpritePtr spr, const sf::Vector2i pos) {
	const sf::Vector2f mousePosF(float(pos.x), float(pos.y));
	const bool result = spr->getSpr()->getGlobalBounds().contains(mousePosF);
	return result;
}

void sfml_cleansingFireApplication::refreshPauseColor() {

	const auto window = getWindowPtr();
	auto gamePtr = getGamePtr();

	const sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
	gamePtr->OnCursorMoved(sfToGamePoint(mousePos));
	const bool mouseOnPauseBtn = isMouseOn(_pauseBtnImg, mousePos) || isMouseOn(_playBtnImg, mousePos);

	auto& playPauseSpr = gamePtr->GetPaused() ? _playBtnImg : _pauseBtnImg;

	const sf::Color playPauseBtnClr = mouseOnPauseBtn ? sf::Color::White : Utils::toSfmlColor(CfgStatic::btnPlayPauseNotHoveredClr);
	playPauseSpr->getSpr()->setColor(playPauseBtnClr);
}

bool sfml_cleansingFireApplication::onMouseMoved(const int x, const int y) {

	const sf::Vector2i mousePos(x, y);
	const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
	_restartBtnImg->getSpr()->setColor(mouseOnRestartBtn ? sf::Color::White : Utils::toSfmlColor(CfgStatic::btnPlayPauseNotHoveredClr));
	refreshPauseColor();
	return false;
}

GamePtr sfml_cleansingFireApplication::createGame(const GameInitParams params) {

	auto gamePtr = std::make_shared<sfml_cleansingFireGame>(getWindowPtr().get());

	if (params.count(skipIntroParam) > 0) {
		gamePtr->SetSkipIntro();
	}

	return gamePtr;
}

bool sfml_cleansingFireApplication::onMousePressed(const int x, const int y) {

	const sf::Vector2i mousePos(x, y);

	const bool mouseOnRestartBtn = isMouseOn(_restartBtnImg, mousePos);
	if (mouseOnRestartBtn) {
		freeResources();
		startGame({ {skipIntroParam, "1"} });

		refreshPauseColor();
		return true;
	}

	const bool mouseOnPauseBtn = isMouseOn(_pauseBtnImg, mousePos);
	if (mouseOnPauseBtn) {
		const bool wasPaused = getGamePtr()->GetPaused();
		getGamePtr()->SetPaused(!wasPaused);
		refreshPauseColor();
		return true;
	}
	return false;
}

bool sfml_cleansingFireApplication::initSpecial() {

	_pauseBtnImg   = initBtn(CfgStatic::pauseImgFile);
	_playBtnImg    = initBtn(CfgStatic::playImgFile);
	_restartBtnImg = initBtn(CfgStatic::restartImgFile);

	if (_pauseBtnImg   == nullptr ||
		_playBtnImg    == nullptr ||
		_restartBtnImg == nullptr) {

		Log::Inst()->PutErr("sfml_cleansingFireApplication::initSpecial error, buttons not initialized... ");
		return false;
	}

	_pauseBtnImg  ->getSpr()->setPosition(0.f, 0.f);
	_playBtnImg   ->getSpr()->setPosition(0.f, 0.f);
	_restartBtnImg->getSpr()->setPosition(CfgStatic::windowSize.getX() - _restartBtnImg->getSpr()->getTextureRect().width, 0.f);

	return true;
}

void sfml_cleansingFireApplication::drawSpecial() {
	auto& playPauseSpr = getGamePtr()->GetPaused() ? _playBtnImg : _pauseBtnImg;

	getWindowPtr()->draw(*playPauseSpr->getSpr());
	getWindowPtr()->draw(*_restartBtnImg->getSpr());
}

