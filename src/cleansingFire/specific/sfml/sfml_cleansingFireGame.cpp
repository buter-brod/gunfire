#include "sfml_cleansingFireGame.h"
#include "Log.h"
#include "Config.h"

#include "CFGameplayComponent.h"

#include "sfml_Text.h"
#include "sfml_ResourceManager.h"
#include "sfml_Sprite.h"

#include "SFML/Graphics/RenderWindow.hpp"

sfml_cleansingFireGame::sfml_cleansingFireGame(sf::RenderWindow* rWnd) : sfml_Game(rWnd) {
	Log::Inst()->PutMessage("sfml_cleansingFireGame");
}

sfml_cleansingFireGame::~sfml_cleansingFireGame() {
	Log::Inst()->PutMessage("~sfml_cleansingFireGame");
}

bool sfml_cleansingFireGame::update(const float dt) {

	const bool updated = sfml_Game::update(dt);
	if (updated) {
		updateText();
		return true;
	}

	return false;
}

void sfml_cleansingFireGame::Init() {

	Log::Inst()->PutMessage("sfml_cleansingFireGame::Init");
	sfml_ResourceManager::Inst()->AddTexture(CfgStatic::bgSprName);
	sfml_ResourceManager::Inst()->AddShader(CfgStatic::pixelizeShader);

	_gameplayPtr = addGameplayComponent<CFGameplayComponent>();

	initText();
	sfml_Game::Init();
}

void sfml_cleansingFireGame::updateText() {

	const float timeRemainSec = _gameplayPtr->GetTimeRemain();

	_timerTxt->setString(CfgStatic::timerTxt + std::to_string(int(timeRemainSec)));
	_scoreTxt->setString(CfgStatic::scoreTxt + std::to_string(_gameplayPtr->GetFrags()));
}

void sfml_cleansingFireGame::Draw() {

	sf::RenderWindow* wnd = getRenderWindow();

	if (wnd == nullptr) {
		Log::Inst()->PutMessage("Game::Draw error, unable to get SFML renderWindow");
		return;
	}

	sfml_Game::Draw();

	wnd->draw(*_scoreTxt);

	if (_gameplayPtr->GetTimeRemain() > 0) {
		wnd->draw(*_timerTxt);
	}
	else {
		wnd->draw(*_gameOverText);
	}
}

void sfml_cleansingFireGame::initText() {

	if (!_font) {
		_font = std::make_shared<Font>();
		_font->loadFromFile(CfgStatic::fontName);
	}

	if (!_timerTxt) {
		_timerTxt = std::make_shared<Text>();
		_timerTxt->setFont(*_font);
		_timerTxt->setPosition(CfgStatic::timerPositionX, CfgStatic::timerPositionY);
	}

	if (!_scoreTxt) {
		_scoreTxt = std::make_shared<Text>();
		_scoreTxt->setFont(*_font);
		_scoreTxt->setFillColor(Utils::toSfmlColor(CfgStatic::scoreClr));
		_scoreTxt->setPosition(CfgStatic::scorePositionX, CfgStatic::scorePositionY);
	}

	if (!_gameOverText) {
		_gameOverText = std::make_shared<Text>();
		_gameOverText->setFont(*_font);

		const std::string& gameOverText = Config::Inst()->getString("gameOverText");
		_gameOverText->setString(gameOverText);
		_gameOverText->setPosition(CfgStatic::gameSize.getX() / 2.f, CfgStatic::gameSize.getY() / 2.f);
	}
}