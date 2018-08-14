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

	Config::Inst();

	Log::Inst()->PutMessage("Game::Game");
	Init();
}

Game::~Game(){

	Log::Inst()->PutMessage("Game::~Game");
}

Size Game::GetSize() const {
	return CfgStatic::gameSize;
}

time_us Game::getTimeRemain() const {

	const auto currClock = Utils::getTime();
	const time_us elapsed_s = (currClock - _startTimetamp) / (1000 * 1000);

	const unsigned int roundDuration = Config::Inst()->getInt("roundDuration");

	if (roundDuration > elapsed_s) {
		const auto timeRemain = roundDuration - elapsed_s;
		return timeRemain;
	}
	
	return 0;
}

void Game::updateText() {

	const time_us timeRemainSec = getTimeRemain();

	_timerTxt->setString(CfgStatic::timerTxt + std::to_string(int(timeRemainSec)));
	_scoreTxt->setString(CfgStatic::scoreTxt + std::to_string(_frags));
}

void Game::Update(const float dt) {

	const auto currClock = Utils::getTime();

	if (_startTimetamp == 0) {
		_startTimetamp = currClock;
	}

	const time_us timeRemainSec = getTimeRemain();

	if (timeRemainSec == 0) {
		return;
	}

	updateText();

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

	wnd->draw(*_scoreTxt);

	if (getTimeRemain() > 0) {
		wnd->draw(*_timerTxt);
	}
	else {
		wnd->draw(*_gameOverText);
	}
}

bool Game::isObjectObsolete(GameObjectPtr objPtr) {

	if (objPtr->GetState() == CfgStatic::deadStateName) {
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

	unsigned int enemyCount = Config::Inst()->getInt("enemyCount");

	if (_enemyObjects.size() >= enemyCount) {
		return;
	}

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;
	const bool leftSide = Utils::rndYesNo();
	
	const float fromX = leftSide ? (-CfgStatic::enemyPositionGapX) : (gameSize.getX() + CfgStatic::enemyPositionGapX);
	const float fromY = Utils::rndfMinMax(CfgStatic::enemyPositionGapYMin, CfgStatic::enemyPositionGapYMax);

	const Point from(fromX, fromY);
	const Point whereTo(center.getX(), fromY);
	const Point direction(whereTo - from);

	const float enemySpeedMin = Config::Inst()->getFloat("enemySpeedMin");
	const float enemySpeedMax = Config::Inst()->getFloat("enemySpeedMax");

	const float speed = Utils::rndfMinMax(enemySpeedMin, enemySpeedMax);

	GameObjectPtr enemyObj = std::make_shared<GameObject>(newID(), CfgStatic::enemyName, CfgStatic::enemySpr);
	enemyObj->AddAnimation(CfgStatic::enemySpr, CfgStatic::enemyAnimFramesCount, CfgStatic::enemyAnimFPS);

	const bool spawnSound = Utils::rnd0xi(enemyCount) == 0;
	if (spawnSound) {
		const unsigned int soundInd = Utils::rnd0xi((unsigned int)CfgStatic::enemySounds.size());
		enemyObj->getIdleState()->_sound = CfgStatic::enemySounds[soundInd];
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

			if (bulletPtr->GetState() == CfgStatic::idleStateName) {
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

	enemy->SetSpeed(CfgStatic::boomAcceleration);
	enemy->SetAngleSpeed((Utils::rndYesNo() ? 1.f : -1.f) *  CfgStatic::boomAngleSpeed);

	GameObject::StatePtr boomState = std::make_shared<GameObject::State>(CfgStatic::boomStateName);
	boomState->_animation = CfgStatic::boomSpr;
	boomState->_duration = basedOnAnimation;
	boomState->_sound = CfgStatic::boomSounds[Utils::rnd0xi((unsigned int)CfgStatic::boomSounds.size())];
	
	GameObject::StatePtr deadState = std::make_shared<GameObject::State>(CfgStatic::deadStateName);
	boomState->_nextState = deadState;

	bullet->ChangeState(boomState);

	++_frags;
}

void Game::tryShoot(const Point& whereTo) {

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - CfgStatic::bulletPositionGapY);
	const Point direction(whereTo - from);

	const std::string& playerStateName = _playerObj->GetState();

	if (direction.getY() > 0 || playerStateName != CfgStatic::idleStateName) {
		return;
	}

	const float speed = Utils::rndfMinMax(CfgStatic::bulletSpeedMin, CfgStatic::bulletSpeedMax);
	const float angleSpeed = Utils::rndfMinMax(CfgStatic::bulletAngleSpeedMin, CfgStatic::bulletAngleSpeedMax);

	GameObjectPtr bottleObj = std::make_shared<GameObject>(newID(), CfgStatic::bulletName, CfgStatic::bulletSpr);

	bottleObj->AddAnimation(CfgStatic::bulletSpr, CfgStatic::bulletAnimFramesCount, CfgStatic::bulletAnimFPS);
	bottleObj->AddAnimation(CfgStatic::boomSpr, CfgStatic::boomAnimFramesCount, CfgStatic::boomAnimFPS);

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);
	bottleObj->SetSpeed(speed);
	bottleObj->SetAngleSpeed(angleSpeed);

	addObject(bottleObj, _bulletObjects);

	GameObject::StatePtr shootState = std::make_shared<GameObject::State>(CfgStatic::shootingStateName);
	shootState->_animation = CfgStatic::playerFireSpr;
	shootState->_duration = CfgStatic::throwDuration;
	shootState->_sound = CfgStatic::throwSound;

	GameObject::StatePtr cooldownState = std::make_shared<GameObject::State>(CfgStatic::cooldownStateName);
	cooldownState->_animation = CfgStatic::playerCooldownSpr;
	cooldownState->_duration = CfgStatic::cooldown;
	cooldownState->_soundEnd = CfgStatic::readySound;

	shootState->_nextState = cooldownState;

	_playerObj->ChangeState(shootState);
}


void Game::initText() {

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
		_scoreTxt->setFillColor(CfgStatic::scoreColor);
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

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	initText();

	for (auto& s : CfgStatic::boomSounds) {
		ResourceManager::Inst()->AddSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		ResourceManager::Inst()->AddSound(s);
	}	
	
	ResourceManager::Inst()->AddSound(CfgStatic::readySound);
	ResourceManager::Inst()->AddSound(CfgStatic::throwSound);

	auto ambientSnd = ResourceManager::Inst()->AddSound(CfgStatic::ambientSound);
	auto musicSnd   = ResourceManager::Inst()->AddSound(CfgStatic::musicTrack);

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
	
	ResourceManager::Inst()->AddTexture(CfgStatic::bgSprName);

	_bgObject = std::make_shared<GameObject>(newID(), CfgStatic::bgName, CfgStatic::bgSprName);
	_bgObject->SetPosition(center);

	_playerObj = std::make_shared<GameObject>(newID(), CfgStatic::playerName, CfgStatic::playerSpr);
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - CfgStatic::playerPositionGapY });
	_playerObj->AddAnimation(CfgStatic::playerFireSpr);
	_playerObj->AddAnimation(CfgStatic::playerCooldownSpr);
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