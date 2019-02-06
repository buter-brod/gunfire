#include "CleansingFireGame.h"

#include "Config.h"
#include "Log.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "SoundManager.h"

#include <algorithm>

CleansingFireGame::CleansingFireGame(){
	Log::Inst()->PutMessage("CleansingFireGame::CleansingFireGame");
}

CleansingFireGame::~CleansingFireGame(){
	Log::Inst()->PutMessage("CleansingFireGame::~CleansingFireGame");
}

float CleansingFireGame::getTimeRemain() const {

	const float elapsed_s = getSimulationTime();
	const unsigned int roundDuration = Config::Inst()->getInt("roundDuration");

	const unsigned int roundWithBonusTime = roundDuration + (unsigned int)_bonusTime;

	if (roundWithBonusTime > elapsed_s) {
		const auto timeRemain = roundWithBonusTime - elapsed_s;
		return timeRemain;
	}
	
	return 0;
}

void CleansingFireGame::updateText() {}

void CleansingFireGame::checkAllObjectsObsolete() {

	checkObjectsObsolete(_enemyObjects);
	checkObjectsObsolete(_bulletObjects);
}

bool CleansingFireGame::updateSpecific(const float dt) {

	const float timeRemainSec = getTimeRemain();

	if (timeRemainSec == 0.f) {
		return false;
	}

	if (_shootRequest.requested) {
		tryShoot(_shootRequest.targetPt);
		_shootRequest = { false, Point() };
	}

	updateText();
	checkSpawn();
	checkCollisions();

	return true;
}

bool CleansingFireGame::isObjectObsolete(GameObjectPtr objPtr) {

	if (Game::isObjectObsolete(objPtr)) {
		return true;
	}

	if (objPtr->GetState() == CfgStatic::leavingStateName) {

		// don't touch me yet, I'm doing something before I can be finally removed!
		return false;
	}

	const Point& pos = objPtr->GetPosition();
	const Point& dir = objPtr->GetDirection();
	const Point& sz  = objPtr->GetSize();

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	// object is either not moving or moving to somewhere far from center
	const bool movingOut = (pos + dir - center).len() >= (pos - center).len();

	if (movingOut) {
		// not visible by game bounds
		const bool outOfBounds =
			pos.getX() < -sz.getX() ||
			pos.getY() < -sz.getY() ||

			pos.getX() > gameSize.getX() + sz.getX() ||
			pos.getY() > gameSize.getY() + sz.getY();

		if (outOfBounds) {
			const bool canBeDestroyedNow = objPtr->RequestKill("flying out of bounds");

			if (canBeDestroyedNow) {
				return true;
			}
		}
	}

	return false;
}

void CleansingFireGame::checkSpawn() {

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

void CleansingFireGame::checkCollisions() {

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

void CleansingFireGame::onCollision(GameObjectPtr bullet, GameObjectPtr enemy) {
	
	BulletPtr bulletPtr = std::static_pointer_cast<Bullet>(bullet);
	EnemyPtr enemyPtr = std::static_pointer_cast<Enemy>(enemy);
	
	enemyPtr->Boom(bulletPtr->GetPosition());
	bulletPtr->Boom();	

	++_frags;

	const float bonusTimePerFrag = Config::Inst()->getFloat("bonusTimePerFrag");

	_bonusTime += bonusTimePerFrag;
}

bool CleansingFireGame::tryShoot(const Point& whereTo) {

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - CfgStatic::playerSize.getY());
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

void CleansingFireGame::initText() {}

void CleansingFireGame::initSound() {

	for (auto& s : CfgStatic::boomSounds) {
		SoundManager::LoadSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		SoundManager::LoadSound(s);
	}	

	SoundManager::LoadSound(CfgStatic::readySound);
	SoundManager::LoadSound(CfgStatic::throwSound);

	SoundManager::LoadSound(CfgStatic::ambientSound);
	SoundManager::PlaySound(CfgStatic::ambientSound, true);

	const bool musicDisabled = Config::Inst()->getInt("noMusic") > 0;

	if (!musicDisabled) {
		SoundManager::LoadSound(CfgStatic::musicTrack);
		SoundManager::PlaySound(CfgStatic::musicTrack, true);
	}
}

void CleansingFireGame::Init() {

	Log::Inst()->PutMessage("CleansingFireGame::Init");

	initText();
	initSound();

	const Size& gameSize = GetSize();
	const Point center = gameSize / 2.f;

	_bgObject = std::make_shared<GameObject>(newID(), CfgStatic::bgName, CfgStatic::bgSprName);
	_bgObject->SetSize(gameSize);
	_bgObject->SetPosition(center);
	addObject(_bgObject);

	_playerObj = std::make_shared<Player>(newID());
	_playerObj->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - CfgStatic::playerSize.getY() / 2.f });
	addObject(_playerObj);

	Game::Init();
}

void CleansingFireGame::OnCursorMoved(const Point& pt) {

	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.strInt());

	if (GetPaused()) {
		return;
	}

	const bool onLeftSide = pt.getX() < GetSize().getX() / 2.f;
	_playerObj->SetMirrorX(onLeftSide);
}

void CleansingFireGame::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.strInt());

	if (GetPaused()) {
		return;
	}

	_shootRequest = { true, pt };
}