#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>

const std::string bgSprName = "clouds.png";

Game::Game(){
	Init();
}

Game::~Game(){
}

void Game::Draw(sf::RenderWindow* wnd) {

	wnd->draw(_bgSprite);
}

void Game::Init() {

	const TexturePtr bgTexPtr = _textureManager.addTexture(bgSprName);

	if (bgTexPtr) {
		_bgSprite.setTexture(*bgTexPtr);
	}
}
