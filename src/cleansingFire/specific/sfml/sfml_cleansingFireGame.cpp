#include "sfml_cleansingFireGame.h"
#include "Log.h"
#include "Config.h"
#include "Player.h"

#include "sfml_Text.h"
#include "sfml_ResourceManager.h"
#include "sfml_Sprite.h"

#include "SFML/Graphics/RenderWindow.hpp"

sfml_cleansingFireGame::sfml_cleansingFireGame(sf::RenderWindow* rWnd) : sfml_Game(rWnd) {
	Log::Inst()->PutMessage("sfml_Game");
}

sfml_cleansingFireGame::~sfml_cleansingFireGame() {
	Log::Inst()->PutMessage("~sfml_Game");
}

void sfml_cleansingFireGame::Init() {

	Log::Inst()->PutMessage("sfml_cleansingFireGame::Init");
	sfml_ResourceManager::Inst()->AddTexture(CfgStatic::bgSprName);
	sfml_ResourceManager::Inst()->AddShader(CfgStatic::pixelizeShader);

	CleansingFireGame::Init();
	sfml_Game::Init();
}

void sfml_cleansingFireGame::updateText() {

	const float timeRemainSec = getTimeRemain();

	_timerTxt->setString(CfgStatic::timerTxt + std::to_string(int(timeRemainSec)));
	_scoreTxt->setString(CfgStatic::scoreTxt + std::to_string(getFrags()));
}

void sfml_cleansingFireGame::drawSpecific() {

	sf::RenderWindow* wnd = getRenderWindow();

	if (wnd == nullptr) {
		Log::Inst()->PutMessage("Game::Draw error, unable to get SFML renderWindow");
		return;
	}

	drawObject(getBGObject());
	drawObject(getPlayerObject());

	for (const auto& obj : getEnemyObjects())  { drawObject(obj); }
	for (const auto& obj : getBulletObjects()) { drawObject(obj); }

	wnd->draw(*_scoreTxt);

	if (getTimeRemain() > 0) {
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