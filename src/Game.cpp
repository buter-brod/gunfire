#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "TextureManager.h"
#include "Animation.h"

const std::string bgSprName = "clouds.png";

IDType Game::newID(){
	return _nextID++;
}

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

void Game::Update() {

	const auto currClock = clock();

	const float dt = Utils::dt(currClock, _clock);

	for (const auto& obj : _objects) {
		obj.second->update(dt);
	}

	_clock = clock();
}

void Game::Draw(sf::RenderWindow* wnd) {

	if (_bgSpritePtr) {
		wnd->draw(*_bgSpritePtr->getSpr());
	}

	for (const auto& obj : _objects) {

		const auto& spr = obj.second->getSprite();
		if (spr) {
			wnd->draw(*spr->getSpr());
		}
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

	GameObjectPtr bottleObj = std::make_shared<GameObject>(newID(), "bottle_fire.png");
	bottleObj->SetPosition({ 300, 300 });
	_objects[bottleObj->getId()] = bottleObj;
}

void Game::OnCursorMoved(const Point& pt) {
	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.str());
}

void Game::OnCursorClicked(const Point& pt) {

	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.str());
}