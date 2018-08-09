#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Config.h"
#include "Log.h"

const std::string bgSprName = "clouds.png";

Game::Game(){
	Init();
}

Game::~Game(){
}

Size Game::getSize() const {
	return Config::gameSize;
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

void Game::OnCursorMoved(const Point& pt) {
	Log::inst()->PutMessage(Log::Channel::VERBOSE, "OnCursorMoved " + pt.str());
}

void Game::OnCursorClicked(const Point& pt) {

	Log::inst()->PutMessage("OnCursorClicked " + pt.str());
}