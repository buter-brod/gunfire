#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "TextureManager.h"
#include "Animation.h"

const std::string bgSprName = "clouds.png";

Game::Game(){

	Log::Inst()->PutMessage("Game::Game");
	Init();
}

Game::~Game(){

	Log::Inst()->PutMessage("Game::~Game");
}

Size Game::GetSize() const {
	return Config::gameSize;
}

void Game::Draw(sf::RenderWindow* wnd) {

	if (_bgSpritePtr) {
		wnd->draw(*_bgSpritePtr->getSpr());
	}
}

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	if (_bgSpritePtr == nullptr) {
	
		const TexturePtr bgTexPtr = TextureManager::Inst()->AddTexture(bgSprName);

		if (bgTexPtr) {
			_bgSpritePtr = std::make_shared<Sprite>(*bgTexPtr);
		}
	}
}

void Game::OnCursorMoved(const Point& pt) {
	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.str());
}

void Game::OnCursorClicked(const Point& pt) {

	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.str());
}