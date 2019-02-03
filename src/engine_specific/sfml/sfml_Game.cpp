#include "sfml_Game.h"
#include "Log.h"
#include "Config.h"
#include "Player.h"

#include "sfml_Text.h"
#include "sfml_EngineComponent.h"
#include "sfml_Particles.h"
#include "sfml_Shader.h"
#include "sfml_ResourceManager.h"
#include "sfml_Sprite.h"

#include "SFML/Graphics/RenderWindow.hpp"

sfml_Game::sfml_Game(sf::RenderWindow* rWnd) : _renderWindow(rWnd) {
	Log::Inst()->PutMessage("sfml_Game");
}

sfml_Game::~sfml_Game() {
	Log::Inst()->PutMessage("~sfml_Game");
}

void sfml_Game::Init() {

	Log::Inst()->PutMessage("sfml_Game::Init");
	sfml_ResourceManager::Inst()->AddTexture(CfgStatic::bgSprName);
	sfml_ResourceManager::Inst()->AddShader(CfgStatic::pixelizeShader);

	Game::Init();
}

bool sfml_Game::update(const float dt) {

	const bool updated = Game::update(dt);

	if (!updated) {
		return false;
	}

	for (auto& objPair : getAllObjects()) {
		auto& objW = objPair.second;
		const auto obj = objW.lock();

		auto engineComponent = obj->GetEngineComponent();
		if (!engineComponent) {
			engineComponent = std::make_shared<sfml_EngineComponent>(obj);
			obj->SetEngineComponent(engineComponent);
			engineComponent->Update(dt);
		}
	}

	return true;
}

void sfml_Game::updateText() {

	const float timeRemainSec = getTimeRemain();

	_timerTxt->setString(CfgStatic::timerTxt + std::to_string(int(timeRemainSec)));
	_scoreTxt->setString(CfgStatic::scoreTxt + std::to_string(getFrags()));
}

void sfml_Game::Draw() {

	sf::RenderWindow* wnd = _renderWindow;

	if (wnd == nullptr) {
		Log::Inst()->PutMessage("Game::Draw error, unable to get SFML renderWindow");
		return;
	}

	auto drawObj = [&wnd, this](GameObjectPtr obj) {

		auto engineComponent = obj->GetEngineComponent();
		sfml_EngineComponent* sfmlComponent = static_cast<sfml_EngineComponent*>(engineComponent.get());

		if (!sfmlComponent) {
			Log::Inst()->PutErr("sfml_Game::Draw drawObj error, unable to get sfmlComponent for " + obj->getFullName());
			return;
		}

		SpritePtr spr = sfmlComponent->GetSprite();
		if (spr) {
			sf::Shader* shader{ nullptr };
			const auto& shPtr = obj->GetShader();
			if (shPtr) {
				auto sfml_sh_ptr = std::static_pointer_cast<sfml_Shader>(shPtr);
				shader = sfml_sh_ptr->get();
			}

			wnd->draw(*spr->getSpr(), shader);
		}

		const auto& particles = sfmlComponent->GetParticles();
		if (particles) {
			wnd->draw(*particles);
		}
	};

	drawObj(getBGObject());
	drawObj(getPlayerObject());

	for (const auto& obj : getEnemyObjects())  { drawObj(obj); }
	for (const auto& obj : getBulletObjects()) { drawObj(obj); }

	wnd->draw(*_scoreTxt);

	if (getTimeRemain() > 0) {
		wnd->draw(*_timerTxt);
	}
	else {
		wnd->draw(*_gameOverText);
	}
}


void sfml_Game::initText() {

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