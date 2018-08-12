#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "ResourceManager.h"
#include "Animation.h"

#include <SFML/Graphics/RenderWindow.hpp>

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

	checkCollisions();
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

	if (objPtr->GetState() == Config::deadStateName) {
		return true;
	}

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

	GameObjectPtr enemyObj = std::make_shared<GameObject>(newID(), Config::enemyName, Config::enemySpr);
	enemyObj->AddAnimation(Config::enemySpr, Config::enemyAnimFramesCount, Config::enemyAnimFPS);

	const bool spawnSound = Utils::rnd0xi(Config::enemyCount) == 0;
	if (spawnSound) {
		const unsigned int soundInd = Utils::rnd0xi((unsigned int)Config::enemySounds.size());
		enemyObj->getIdleState()->_sound = Config::enemySounds[soundInd];
	}

	enemyObj->SetMirrorX(leftSide);

	enemyObj->SetPosition(from);
	enemyObj->SetDirection(direction);
	enemyObj->SetSpeed(speed);
	
	addObject(enemyObj, _enemyObjects);
}

bool collides(GameObjectPtr obj1, GameObjectPtr obj2) {

	const Point& p1 = obj1->GetPosition();
	const Point& p2 = obj2->GetPosition();

	const float d = (p1 - p2).len();

	const Size& s1 = obj1->GetSize();
	const Size& s2 = obj2->GetSize();
	
	const float minDist = (s1 + s2).len() / 4.f;
	const bool collide = d < minDist;

	return collide;
}

void Game::checkCollisions() {

	for (const auto& bulletPtr : _bulletObjects) {
		for (const auto& enemyPtr : _enemyObjects) {

			if (bulletPtr->GetState() == Config::idleStateName) {
				if (collides(bulletPtr, enemyPtr)) {
					onCollision(bulletPtr, enemyPtr);
				}
			}			
		}
	}
}

void Game::onCollision(GameObjectPtr bullet, GameObjectPtr enemy) {

	Point enemyDirection = enemy->GetDirection().normalized();
	enemyDirection.Y() = -1.f;
	enemy->SetDirection(enemyDirection);

	enemy->SetSpeed(Config::boomAcceleration);
	enemy->SetAngleSpeed((Utils::rndYesNo() ? 1.f : -1.f) *  Config::boomAngleSpeed);

	GameObject::StatePtr boomState = std::make_shared<GameObject::State>(Config::boomStateName);
	boomState->_animation = Config::boomSpr;
	boomState->_duration = basedOnAnimation;
	boomState->_sound = Config::boomSounds[Utils::rnd0xi((unsigned int)Config::boomSounds.size())];
	
	GameObject::StatePtr deadState = std::make_shared<GameObject::State>(Config::deadStateName);
	boomState->_nextState = deadState;

	bullet->ChangeState(boomState);
}

void Game::tryShoot(const Point& whereTo) {

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - Config::bulletPositionGapY);
	const Point direction(whereTo - from);

	const std::string& playerStateName = _playerObj->GetState();

	if (direction.getY() > 0 || playerStateName != Config::idleStateName) {
		return;
	}

	const float speed = Utils::rndfMinMax(Config::bulletSpeedMin, Config::bulletSpeedMax);
	const float angleSpeed = Utils::rndfMinMax(Config::bulletAngleSpeedMin, Config::bulletAngleSpeedMax);

	GameObjectPtr bottleObj = std::make_shared<GameObject>(newID(), Config::bulletName, Config::bulletSpr);

	bottleObj->AddAnimation(Config::bulletSpr, Config::bulletAnimFramesCount, Config::bulletAnimFPS);
	bottleObj->AddAnimation(Config::boomSpr, Config::boomAnimFramesCount, Config::boomAnimFPS);

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);
	bottleObj->SetSpeed(speed);
	bottleObj->SetAngleSpeed(angleSpeed);

	addObject(bottleObj, _bulletObjects);

	GameObject::StatePtr shootState = std::make_shared<GameObject::State>(Config::shootingStateName);
	shootState->_animation = Config::playerFireSpr;
	shootState->_duration = Config::throwDuration;
	shootState->_sound = Config::throwSound;

	GameObject::StatePtr cooldownState = std::make_shared<GameObject::State>(Config::cooldownStateName);
	cooldownState->_animation = Config::playerCooldownSpr;
	cooldownState->_duration = Config::cooldown;
	cooldownState->_soundEnd = Config::readySound;

	shootState->_nextState = cooldownState;

	_playerObj->ChangeState(shootState);
}

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	for (auto& s : Config::boomSounds) {
		ResourceManager::Inst()->AddSound(s);
	}

	for (auto& s : Config::enemySounds) {
		ResourceManager::Inst()->AddSound(s);
	}	
	
	ResourceManager::Inst()->AddSound(Config::readySound);
	ResourceManager::Inst()->AddSound(Config::throwSound);

	auto ambientSnd = ResourceManager::Inst()->AddSound(Config::ambientSound);
	auto musicSnd   = ResourceManager::Inst()->AddSound(Config::musicTrack);

	if (ambientSnd) {
		ambientSnd->get().play();
		ambientSnd->get().setLoop(true);
	}

	if (musicSnd) {
		musicSnd->get().play();
		musicSnd->get().setLoop(true);
	}

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;
	
	ResourceManager::Inst()->AddTexture(Config::bgSprName);

	_bgObject = std::make_shared<GameObject>(newID(), Config::bgName, Config::bgSprName);
	_bgObject->SetPosition(center);

	_playerObj = std::make_shared<GameObject>(newID(), Config::playerName, Config::playerSpr);
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - Config::playerPositionGapY });
	_playerObj->AddAnimation(Config::playerFireSpr);
	_playerObj->AddAnimation(Config::playerCooldownSpr);
}

void Game::OnCursorMoved(const Point& pt) {

	const bool onLeftSide = pt.getX() < GetSize().getX() / 2.f;
	_playerObj->SetMirrorX(onLeftSide);

	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.strInt());
}

void Game::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.strInt());

	tryShoot(pt);
}