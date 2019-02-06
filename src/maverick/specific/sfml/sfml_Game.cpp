#include "sfml_Game.h"
#include "Log.h"
#include "Config.h"
#include "GameObject.h"

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

void sfml_Game::drawSpecific() {}

bool sfml_Game::drawObject(GameObjectPtr obj) {

	auto engineComponent = obj->GetEngineComponent();
	sfml_EngineComponent* sfmlComponent = static_cast<sfml_EngineComponent*>(engineComponent.get());

	if (!sfmlComponent) {
		Log::Inst()->PutErr("sfml_Game::Draw drawObj error, unable to get sfmlComponent for " + obj->getFullName());
		return false;
	}

	SpritePtr spr = sfmlComponent->GetSprite();
	if (spr) {
		sf::Shader* shader{ nullptr };
		const auto& shPtr = obj->GetShader();
		if (shPtr) {
			auto sfml_sh_ptr = std::static_pointer_cast<sfml_Shader>(shPtr);
			shader = sfml_sh_ptr->get();
		}

		_renderWindow->draw(*spr->getSpr(), shader);
	}

	const auto& particles = sfmlComponent->GetParticles();
	if (particles) {
		_renderWindow->draw(*particles);
	}

	return true;
}

void sfml_Game::Draw() {
	drawSpecific();
}
