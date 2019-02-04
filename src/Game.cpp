#include "Game.h"

#include "Config.h"
#include "Log.h"
#include "SoundManager.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"

#include <algorithm>

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

	const unsigned int roundWithBonusTime = roundDuration + (unsigned int)_bonusTime;

	if (roundWithBonusTime > elapsed_s) {
		const auto timeRemain = roundWithBonusTime - elapsed_s;
		return timeRemain;
	}
	
	return 0;
}

void Game::updateText() {
}

void Game::checkAllObjectsObsolete() {

	checkObjectsObsolete(_enemyObjects);
	checkObjectsObsolete(_bulletObjects);
}

void Game::updateAllObjects(const float dt) {

	for (auto& objW : _allObjects) {
		auto obj = objW.second.lock();
		
		if (!obj) {
			Log::Inst()->PutErr("Game::checkObjectsObsolete error, " + obj->getFullName() + " not found?! ");
			continue;
		}

		obj->Update(dt, _simulationTime);
	}
}

bool Game::update(const float dt) {
	if (_paused) {
		return false;
	}

	_simulationTime += dt;

	const float timeRemainSec = getTimeRemain();

	if (timeRemainSec == 0.f) {
		return false;
	}

	if (_shootRequest.requested) {
		tryShoot(_shootRequest.targetPt);
		_shootRequest = { false, Point() };
	}

	updateText();
	checkAllObjectsObsolete();
	checkSpawn();
	checkCollisions();
	updateAllObjects(dt);

	return true;
}

bool Game::Update(const float frameDt) {
	
	bool updated = false;
	const float dt = 1.f / CfgStatic::simulationFPS;
	_simulationTimeAcc += frameDt;

	while (_simulationTimeAcc >= dt) {
		update(dt);
		_simulationTimeAcc -= dt;
		updated |= true;
	}

	return updated;
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

bool Game::checkObjectsObsolete(ObjectsArr& arr) {

	std::set<IDType> obsoletes;

	for (const auto& obj : arr) {
		if (isObjectObsolete(obj)) {
			obsoletes.insert(obj->getId());
		}
	}

	if (obsoletes.empty()) {
		return false;
	}

	for (const IDType obsId : obsoletes) {
		const auto objIt = std::find_if(arr.begin(), arr.end(), [obsId](const ObjectsArr::value_type& obj) { return obj->getId() == obsId; });

		if (objIt != arr.end()) {
			removeObject(*objIt, arr);			
		}
		else {
			Log::Inst()->PutErr("Game::checkObjectsObsolete error, id " + std::to_string(obsId) + " not found?! ");
		}
	}

	return true;
}

GameObjectPtr Game::GetObject(const IDType id) const {

	const auto& objWPtrIt = _allObjects.find(id);

	if (objWPtrIt != _allObjects.end()) {

		auto existObjectPtr = objWPtrIt->second.lock();
		if (existObjectPtr) { // object is alive
			return existObjectPtr;
		}
		else {
			Log::Inst()->PutErr("Game::GetObject error, id " + std::to_string(id) + " found in registry but object does not exist!");
		}
	}
	else {
		// that's ok, object with that id may not exist in game
	}

	return GameObjectPtr();
}

bool Game::addObject(GameObjectPtr objPtr) {

	const IDType id = objPtr->getId();

	if (_allObjects.count(id) == 0) {
		_allObjects[id] = objPtr;
	}
	else {
		Log::Inst()->PutErr("Game::addObject error, _allObjects already has id " + std::to_string(id));
		return false;
	}

	return true;
}

bool Game::addObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);
	
	if (objIt != arr.end()) {
		return false;
	}

	arr.push_back(objPtr);
	const bool added = addObject(objPtr);
	return added;
}

bool Game::removeObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);

	if (objIt == arr.end()) {
		return false;
	}

	const IDType id = objPtr->getId();

	if (_allObjects.count(id) == 1) {
		_allObjects.erase(id);
	}
	else {
		Log::Inst()->PutErr("Game::removeObject error, _allObjects hasn't id " + std::to_string(id));
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

	GameObjectPtr enemyObj = std::make_shared<Enemy>(newID());

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
	
	static const float minDistCoeff = 4.f;

	const float minDist = (s1 + s2).len() / minDistCoeff;
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

	const float bonusTimePerFrag = Config::Inst()->getFloat("bonusTimePerFrag");

	_bonusTime += bonusTimePerFrag;
}

bool Game::tryShoot(const Point& whereTo) {

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - CfgStatic::bulletPositionGapY);
	const Point direction(whereTo - from);

	const std::string& playerStateName = _playerObj->GetState();

	if (direction.getY() > 0 || playerStateName != CfgStatic::idleStateName) {
		return false;
	}

	GameObjectPtr bottleObj = std::make_shared<Bullet>(newID());

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);

	addObject(bottleObj, _bulletObjects);

	_playerObj->Shoot();

	return true;
}

void Game::initText() {
}

void Game::initSound() {

	for (auto& s : CfgStatic::boomSounds) {
		SoundManager::Inst()->LoadSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		SoundManager::Inst()->LoadSound(s);
	}	

	SoundManager::Inst()->LoadSound(CfgStatic::readySound);
	SoundManager::Inst()->LoadSound(CfgStatic::throwSound);

	SoundManager::Inst()->LoadSound(CfgStatic::ambientSound);
	SoundManager::Inst()->PlaySound(CfgStatic::ambientSound, true);

	const bool musicDisabled = Config::Inst()->getInt("noMusic") > 0;

	if (!musicDisabled) {
		SoundManager::Inst()->LoadSound(CfgStatic::musicTrack);
		SoundManager::Inst()->PlaySound(CfgStatic::musicTrack, true);
	}
}

void Game::Init() {

	Log::Inst()->PutMessage("Game::Init");

	initText();
	initSound();

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	_bgObject = std::make_shared<GameObject>(newID(), CfgStatic::bgName, CfgStatic::bgSprName);
	_bgObject->SetSize(gameSize);
	_bgObject->SetPosition(center);
	addObject(_bgObject);

	_playerObj = std::make_shared<Player>(newID());
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - CfgStatic::playerPositionGapY });
	addObject(_playerObj);

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

	_shootRequest = { true, pt };
}