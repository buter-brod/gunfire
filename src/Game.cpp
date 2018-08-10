#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "TextureManager.h"

const std::string bgSprName = "clouds.png";

Game::Game(){

	Log::inst()->PutMessage("Game::Game");
	Init();
}

Game::~Game(){

	Log::inst()->PutMessage("Game::~Game");
}

Size Game::getSize() const {
	return Config::gameSize;
}

void Game::Draw(sf::RenderWindow* wnd) {

	wnd->draw(_bgSprite);
}

void Game::Init() {

	Log::inst()->PutMessage("Game::Init");

	const TexturePtr bgTexPtr = TextureManager::inst()->addTexture(bgSprName);

	if (bgTexPtr) {
		_bgSprite.setTexture(*bgTexPtr->getTex());
	}
}

void Game::OnCursorMoved(const Point& pt) {
	Log::inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.str());
}

void Game::OnCursorClicked(const Point& pt) {

	Log::inst()->PutMessage("Game::OnCursorClicked " + pt.str());
}