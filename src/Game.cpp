#include "Game.h"
#include <iostream>
#include "Config.h"
#include "Log.h"
#include "ResourceManager.h"
#include "Animation.h"
#include "Enemy.h"
#include "Bullet.h"

IDType Game::newID(){
	return _nextID++;
}

Game::Game(){

	Config::Inst();
	Log::Inst()->PutMessage("Game::Game");
}

Game::~Game(){

	Log::Inst()->PutMessage("Game::~Game");
}

Size Game::GetSize() const {
	return CfgStatic::gameSize;
}

float Game::getTimeRemain() const {

	const float elapsed_s = _simulationTime;
	const unsigned int roundDuration = Config::Inst()->getInt("roundDuration");

	if (roundDuration > elapsed_s) {
		const auto timeRemain = roundDuration - elapsed_s;
		return timeRemain;
	}
	
	return 0;
}

void Game::updateText() {

	const float timeRemainSec = getTimeRemain();

	_timerTxt->setString(CfgStatic::timerTxt + std::to_string(int(timeRemainSec)));
	_scoreTxt->setString(CfgStatic::scoreTxt + std::to_string(_frags));
}

void Game::update(const float dt) {
	if (_paused) {
		return;
	}

	_simulationTime += dt;

	const float timeRemainSec = getTimeRemain();

	if (timeRemainSec == 0.f) {
		return;
	}

	updateText();

	_bgObject->Update(dt);

	_playerObj->Update(dt);

	checkObjectsObsolete(_enemyObjects);
	checkObjectsObsolete(_bulletObjects);
	checkObjectsObsolete(_effectObjects);

	checkSpawn();

	for (const auto& obj : _enemyObjects)  { obj->Update(dt); }
	for (const auto& obj : _bulletObjects) { obj->Update(dt); }
	for (const auto& obj : _effectObjects) { obj->Update(dt); }

	checkCollisions();
}

void Game::Update(const float frameDt) {
	
	const float dt = 1.f / CfgStatic::simulationFPS;
	_simulationTimeAcc += frameDt;

	while (_simulationTimeAcc >= dt) {
		update(dt);
		_simulationTimeAcc -= dt;
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

void Game::checkSpawn() {

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

	GameObjectPtr enemyObj = std::make_shared<Enemy>(newID(), weak_from_this());

	enemyObj->SetMirrorX(leftSide);
	enemyObj->SetPosition(from);
	enemyObj->SetDirection(direction);

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

		const bool bulletCanExplode = (bulletPtr->GetState() == CfgStatic::idleStateName);

		if (bulletCanExplode) {
			for (const auto& enemyPtr : _enemyObjects) {

				const bool enemyCanExplode = (enemyPtr->GetState() == CfgStatic::idleStateName);

				if (enemyCanExplode && collides(bulletPtr, enemyPtr)) {
					onCollision(bulletPtr, enemyPtr);
				}
			}
		}
	}
}

void Game::onCollision(GameObjectPtr bullet, GameObjectPtr enemy) {
	
	BulletPtr bulletPtr = std::static_pointer_cast<Bullet>(bullet);
	EnemyPtr enemyPtr = std::static_pointer_cast<Enemy>(enemy);
	
	enemyPtr->Boom(bulletPtr->GetPosition());
	bulletPtr->Boom();	

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

	GameObjectPtr bottleObj = std::make_shared<Bullet>(newID(), weak_from_this());

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);

	addObject(bottleObj, _bulletObjects);

	_playerObj->Shoot();
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
		_scoreTxt->setFillColor(Utils::toSfmlColor(CfgStatic::scoreClr));
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

void Game::initSound() {

	for (auto& s : CfgStatic::boomSounds) {
		ResourceManager::Inst()->AddSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		ResourceManager::Inst()->AddSound(s);
	}	
	
	ResourceManager::Inst()->AddShader(CfgStatic::pixelizeShader);

	ResourceManager::Inst()->AddSound(CfgStatic::readySound);
	ResourceManager::Inst()->AddSound(CfgStatic::throwSound);

	auto ambientSnd = ResourceManager::Inst()->AddSound(CfgStatic::ambientSound);

	if (ambientSnd) {
		ambientSnd->Play();
		ambientSnd->SetLoop(true);
	}

	const bool musicDisabled = Config::Inst()->getInt("noMusic") > 0;

	if (!musicDisabled) {
		auto musicSnd = ResourceManager::Inst()->AddSound(CfgStatic::musicTrack);

		if (musicSnd) {
			musicSnd->Play();
			musicSnd->SetLoop();
		}
	}
}

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	initText();
	initSound();

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;
	
	ResourceManager::Inst()->AddTexture(CfgStatic::bgSprName);

	_bgObject = std::make_shared<GameObject>(newID(), CfgStatic::bgName, CfgStatic::bgSprName, weak_from_this());
	_bgObject->SetPosition(center);

	_playerObj = std::make_shared<Player>(newID(), weak_from_this());
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - CfgStatic::playerPositionGapY });

	update(0.f);
}

bool Game::GetPaused() const {
	return _paused; 
}

bool Game::SetPaused(const bool paused) {

	if (_paused == paused) {
		Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::SetPaused, _paused already " + std::string(paused ? "1" : "0"));
		return false;
	}

	_paused = paused;
	return true;
}

void Game::OnCursorMoved(const Point& pt) {

	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.strInt());

	if (_paused) {
		return;
	}

	const bool onLeftSide = pt.getX() < GetSize().getX() / 2.f;
	_playerObj->SetMirrorX(onLeftSide);
}

void Game::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.strInt());

	if (_paused) {
		return;
	}

	tryShoot(pt);
}