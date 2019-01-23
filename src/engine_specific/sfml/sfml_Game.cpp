#include "Game.h"
#include "Log.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "sfml_Shader.h"

sf::RenderWindow*& gameRenderWindow() {
	static sf::RenderWindow* wnd{ nullptr };
	return wnd;
}

void setGameRenderWindow(sf::RenderWindow* wnd) {
	gameRenderWindow() = wnd;
}

void Game::Draw() {

	sf::RenderWindow* wnd = gameRenderWindow();

	if (wnd == nullptr) {
		Log::Inst()->PutMessage("Game::Draw error, unable to get SFML renderWindow");
		return;
	}

	auto drawObj = [&wnd](GameObjectPtr obj) {
		const auto& spr = obj->getSprite();
		if (spr) {
			sf::Shader* shader{ nullptr };
			const auto& shPtr = obj->GetShader();
			if (shPtr) {
				auto sfml_sh_ptr = std::static_pointer_cast<sfml_Shader>(shPtr);
				shader = sfml_sh_ptr->get();
			}

			wnd->draw(*obj->getSprite()->getSpr(), shader);
		}

		const auto& particles = obj->getParticles();
		if (particles) {
			wnd->draw(*particles);
		}
	};

	drawObj(_bgObject);
	drawObj(_playerObj);

	for (const auto& obj : _enemyObjects) { drawObj(obj); }
	for (const auto& obj : _bulletObjects) { drawObj(obj); }
	for (const auto& obj : _effectObjects) { drawObj(obj); }

	wnd->draw(*_scoreTxt);

	if (getTimeRemain() > 0) {
		wnd->draw(*_timerTxt);
	}
	else {
		wnd->draw(*_gameOverText);
	}
}