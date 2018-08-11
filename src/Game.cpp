#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "TextureManager.h"
#include "Animation.h"

#include <SFML/Graphics/RenderWindow.hpp>

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

void Game::Update(const float dt) {

	const auto currClock = Utils::getTime();

	_bgObject->Update(dt);
	_playerObj->Update(dt);

	checkObjectsObsolete(_enemyObjects);
	checkObjectsObsolete(_bulletObjects);
	checkObjectsObsolete(_effectObjects);

	spawn();

	for (const auto& obj : _enemyObjects)  { obj->Update(dt); }
	for (const auto& obj : _bulletObjects) { obj->Update(dt); }
	for (const auto& obj : _effectObjects) { obj->Update(dt); }
}

void Game::Draw(sf::RenderWindow* wnd) {

	auto drawObj = [wnd](GameObjectPtr obj) {
		const auto& spr = obj->getSprite();
		if (spr) {
			wnd->draw(*spr->getSpr());
		}
	};

	drawObj(_bgObject);
	drawObj(_playerObj);
		
	for (const auto& obj : _enemyObjects)  { drawObj(obj); }
	for (const auto& obj : _bulletObjects) { drawObj(obj); }
	for (const auto& obj : _effectObjects) { drawObj(obj); }
}

bool Game::isObjectObsolete(GameObjectPtr objPtr) {
	const Point& pos = objPtr->GetPosition();
	const Point& dir = objPtr->GetDirection();
	const Point& sz  = objPtr->GetSize();
	const Point& hsz = sz / 2.f;

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	// object is either not moving or moving to somewhere far from center
	const bool movingOut = (pos + dir - center).len() >= (pos - center).len();

	if (movingOut) {

		// not visible by game bounds
		const bool outOfBounds =
			pos.getX() < -hsz.getX() ||
			pos.getY() < -hsz.getY() ||

			pos.getX() > gameSize.getX() + hsz.getX() ||
			pos.getY() > gameSize.getY() + hsz.getY();

		if (outOfBounds) {
			return true;
		}
	}

	return false;
}

void Game::checkObjectsObsolete(ObjectsArr& arr) {

	arr.erase(std::remove_if(arr.begin(), arr.end(), [this](const GameObjectPtr& obj) { return isObjectObsolete(obj); }), arr.end());
}

bool Game::addObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);
	
	if (objIt != arr.end()) {
		return false;
	}

	arr.push_back(objPtr);
	return true;
}

bool Game::removeObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);

	if (objIt == arr.end()) {
		return false;
	}

	arr.erase(objIt);
	return true;
}

void Game::spawn() {

	if (_enemyObjects.size() >= Config::enemyCount) {
		return;
	}

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;
	const bool leftSide = Utils::rndYesNo();
	
	const float fromX = leftSide ? (-Config::enemyPositionGapX) : (gameSize.getX() + Config::enemyPositionGapX);
	const float fromY = Utils::rndfMinMax(Config::enemyPositionGapYMin, Config::enemyPositionGapYMax);

	const Point from(fromX, fromY);
	const Point whereTo(center.getX(), fromY);
	const Point direction(whereTo - from);

	const float speed = Utils::rndfMinMax(Config::enemySpeedMin, Config::enemySpeedMax);

	GameObjectPtr enemyObj = std::make_shared<GameObject>(newID(), Config::enemySpr);

	//enemyObj->AddAnimation(Config::enemySpr, Config::enemyAnimFramesCount, Config::enemyAnimFPS);

	if (!leftSide) {
		enemyObj->SetMirrorX(true);
	}

	enemyObj->SetPosition(from);
	enemyObj->SetDirection(direction);
	enemyObj->SetSpeed(speed);
	
	addObject(enemyObj, _enemyObjects);
}

void Game::shoot(const Point& whereTo) {

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - Config::bulletPositionGapY);
	const Point direction(whereTo - from);

	const float speed = Utils::rndfMinMax(Config::bulletSpeedMin, Config::bulletSpeedMax);
	const float angleSpeed = Utils::rndfMinMax(Config::bulletAngleSpeedMin, Config::bulletAngleSpeedMax);

	GameObjectPtr bottleObj = std::make_shared<GameObject>(newID(), Config::bulletSpr);

	bottleObj->AddAnimation(Config::bulletSpr, Config::bulletAnimFramesCount, Config::bulletAnimFPS);
	bottleObj->AddAnimation(Config::boomSpr, Config::boomAnimFramesCount, Config::boomAnimFPS);

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);
	bottleObj->SetSpeed(speed);
	bottleObj->SetAngleSpeed(angleSpeed);

	addObject(bottleObj, _bulletObjects);
}

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;
	
	TextureManager::Inst()->AddTexture(bgSprName);

	_bgObject = std::make_shared<GameObject>(newID(), bgSprName);
	_bgObject->SetPosition(center);

	_playerObj = std::make_shared<GameObject>(newID(), "fox");
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - Config::playerPositionGapY });

	_playerObj->AddAnimation(Config::playerSpr, 1, 0);
	_playerObj->AddAnimation(Config::playerFireSpr, 1, 0);
}

void Game::OnCursorMoved(const Point& pt) {

	const bool onLeftSide = pt.getX() < GetSize().getX() / 2.f;
	_playerObj->SetMirrorX(onLeftSide);

	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.str());
}

void Game::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.str());

	shoot(pt);
}