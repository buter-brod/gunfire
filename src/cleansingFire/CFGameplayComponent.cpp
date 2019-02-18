#include "CFGameplayComponent.h"
#include "Log.h"
#include "Config.h"
#include "CfgStatic.h"
#include "CFCfgStatic.h"
#include "SoundManager.h"
#include "Dialog.h"

#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

const std::string& CFGameplayComponent::nameLiteral() {
	static const std::string& cfNameLiteral = "cleansingFire";
	return cfNameLiteral;
};

CFGameplayComponent::CFGameplayComponent(const Size& sz) : GameplayComponent(sz) {
	setName(nameLiteral());
	Log::Inst()->PutMessage("CFGameplayComponent");
}

CFGameplayComponent::~CFGameplayComponent() {
	Log::Inst()->PutMessage("~CFGameplayComponent");

	if (!_musicTrack.empty()) {
		SoundManager::StopSound(_musicTrack);
	}

	if (!_ambientTrack.empty()) {
		SoundManager::StopSound(_ambientTrack);
	}
}

GameObjectArrPtrVec CFGameplayComponent::getObjectLists() {

	if (_dlgMgr->IsActive()) {
		return { &_dlgMgr->GetObjects() };
	}

	return { 
		&_miscObjects, 
		&_enemyObjects, 
		&_bulletObjects };
}

void CFGameplayComponent::setState(const GameState state) {

	_state = state;

	if (_state == GameState::INIT) {
		Log::Inst()->PutErr("CFGameplayComponent::setState error, INIT should not be set explicitly");
	}
	else if (_state == GameState::INTRO) {

		Log::Inst()->PutMessage("CFGameplayComponent setState INTRO");
		_dlgMgr->StartDialog(CfgStatic::introDlgName);
		SoundManager::PlaySound(CfgStatic::messageSound);
	}
	else if (_state == GameState::MAIN) {

		Log::Inst()->PutMessage("CFGameplayComponent setState MAIN");

		if (!_musicTrack.empty()) {
			SoundManager::PlaySound(_musicTrack, true);
		}

		if (!_ambientTrack.empty()) {
			SoundManager::PlaySound(_ambientTrack, true);
		}		
	}
	else if (_state == GameState::GAMEOVER) {

		Log::Inst()->PutMessage("CFGameplayComponent setState GAMEOVER");
		startGameOverAnim();
	}
	else if (_state == GameState::OUTRO) {

		Log::Inst()->PutMessage("CFGameplayComponent setState OUTRO");
		_dlgMgr->StartDialog(CfgStatic::outroDlgName);
		SoundManager::PlaySound(CfgStatic::messageSound);
	}	
}

bool CFGameplayComponent::tryStateAdvance() {

	if (_state == GameState::INIT) {
		setState(_skipIntro ? GameState::MAIN : GameState::INTRO);
		return true;
	}
	else if (_state == GameState::INTRO) {
		if(!_dlgMgr->IsActive()) {
			setState(GameState::MAIN);
			return true;
		}
	}
	else if (_state == GameState::MAIN) {
		if (_timeRemain == 0.f) { 
			setState(GameState::GAMEOVER);
			return true;
		}
	}
	else if (_state == GameState::GAMEOVER) {
		if (_playerWPtr.lock() == nullptr) {
			setState(GameState::OUTRO);
			return true;
		}
	}
	else if (_state == GameState::OUTRO) {
	}

	return false;
}

bool CFGameplayComponent::Update(const float dt) {

	_timeRemain = getTimeRemain();

	if (getPaused()) {
		return false;
	}

	tryStateAdvance();

	const bool mainState = (_state == GameState::MAIN);
	const bool dlgState = (_state == GameState::INTRO || _state == GameState::OUTRO);

	const float t = dlgState ? 0.f : dt;

	if (mainState) {

		checkSpawn();
		checkCollisions();
	}

	GameplayComponent::Update(t);
	return true;
}

bool isMovingOut(const Point& p, const Point& dir) {
	
	/*todo: check if sqrt is fast enough on different platforms!
	may also use this version, if something goes wrong with sqrt perfomance.. (MSVS'17 - ok)

	const auto sign = [](const float val) -> bool {return val >= 0.f; };

	const float px = p.getX();
	const float py = p.getY();

	const float dx = dir.getX();
	const float dy = dir.getY();

	const bool away = 
		(sign(px) == sign(dx)) || 
		(sign(py) == sign(dy));
	 */

	const bool away = p.len() < (p + dir).len();
	return away;
}

bool CFGameplayComponent::isObjectObsolete(GameObjectPtr objPtr) {

	if (objPtr->GetState() == CfgStatic::deadStateName) {
		return true;
	}

	if (objPtr->GetState() == CfgStatic::leavingStateName) {

		// don't touch me yet, I'm doing something before I can be finally removed!
		return false;
	}

	const Point& pos = objPtr->GetPosition();
	const Point& dir = objPtr->GetDirection();
	const Point& sz = objPtr->GetSize();

	const Size& gameSize = CfgStatic::gameSize;

	const bool outOfBounds =
		pos.getX() < -sz.getX() ||
		pos.getY() < -sz.getY() ||

		pos.getX() > gameSize.getX() + sz.getX() ||
		pos.getY() > gameSize.getY() + sz.getY();

	if (outOfBounds) {
		// not visible by game bounds

		const bool notMoving = objPtr->GetSpeed() == 0.f; 
		const Point center = gameSize / 2.f;

		// object is either not moving or moving to somewhere far from center
		const bool isAway = notMoving || isMovingOut((pos - center), dir);

		if (isAway) {
			const bool canBeDestroyedNow = objPtr->RequestKill("flying out of bounds");

			if (canBeDestroyedNow) {
				return true;
			}
		}
	}

	return false;
}

float CFGameplayComponent::getTimeRemain() const {
	const float elapsed_s = getSimulationTime();
	const unsigned int roundDuration = Config::Inst()->getInt("roundDuration");

	const unsigned int roundWithBonusTime = roundDuration + (unsigned int)_bonusTime;

	if (roundWithBonusTime > elapsed_s) {
		const auto timeRemain = roundWithBonusTime - elapsed_s;
		return timeRemain;
	}

	return 0.f;
}

float CFGameplayComponent::GetTimeRemain() const {

	if (_timeRemain < 0.f){
		_timeRemain = getTimeRemain();
	}

	return _timeRemain;
}

bool CFGameplayComponent::isGameOverAnimStarted() const {

	const auto playerPtr = _playerWPtr.lock();

	if (playerPtr) {
		const std::string& playerState = playerPtr->GetState();
		const bool playing = (playerState != CfgStatic::dyingStateName && playerState != CfgStatic::deadStateName);
		return !playing;
	}

	return true;
}

void CFGameplayComponent::setGameOverAnimFor(const GameObjectPtr& obj) const {
	const std::string& currAnim = obj->GetAnimation();

	obj->SetSpeed(0.f);
	obj->SetAngleSpeed(0.f);

	GameObject::StatePtr dyingState = GameObject::State::New(CfgStatic::dyingStateName);
	dyingState->_animation = currAnim; // let it continue latest animation while fading out
	dyingState->_shader = CfgStatic::pixelizeShader;
	dyingState->_duration = CfgStatic::outroEffectDuration;

	if (_bgWPtr.lock() != obj) {
		// we don't need background to be removed
		dyingState->_nextState = GameObject::State::New(CfgStatic::deadStateName);
	}

	obj->ChangeState(dyingState);
}

void CFGameplayComponent::startGameOverAnim() {

	const auto& arrs = getObjectLists();

	for (auto& arr : arrs) {
		for (auto& obj : *arr) {
			setGameOverAnimFor(obj.second);
		}
	}
}

void CFGameplayComponent::checkSpawn() {

	const unsigned int enemyCountShouldBe = Config::Inst()->getInt("enemyCount");

	while (_enemyObjects.size() < enemyCountShouldBe) {
		const Size& gameSize = CfgStatic::gameSize;
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
}

bool collides(const GameObjectPtr& obj1, const GameObjectPtr& obj2) {

	const Point& p1 = obj1->GetPosition();
	const Point& p2 = obj2->GetPosition();

	const float d = (p1 - p2).len();

	const Size& s1 = obj1->GetSize();
	const Size& s2 = obj2->GetSize();

	const float minDist = (s1 + s2).len() / CfgStatic::collideDistCoeff;
	const bool collide = d < minDist;

	return collide;
}

void CFGameplayComponent::checkCollisions() {

	for (const auto& bulletPair : _bulletObjects) {
		const auto& bulletPtr = bulletPair.second;

		const bool bulletCanExplode = (bulletPtr->GetState() == CfgStatic::idleStateName);

		if (bulletCanExplode) {
			for (const auto& enemyPair : _enemyObjects) {
				const auto& enemyPtr = enemyPair.second;

				const bool enemyCanExplode = (enemyPtr->GetState() == CfgStatic::idleStateName);

				if (enemyCanExplode && collides(bulletPtr, enemyPtr)) {
					onCollision(bulletPtr, enemyPtr);
				}
			}
		}
	}
}

void CFGameplayComponent::onCollision(const GameObjectPtr& bullet, const GameObjectPtr& enemy) {

	BulletPtr bulletPtr = std::static_pointer_cast<Bullet>(bullet);
	EnemyPtr enemyPtr = std::static_pointer_cast<Enemy>(enemy);

	enemyPtr->Boom(bulletPtr->GetPosition());
	bulletPtr->Boom();

	++_frags;

	const float bonusTimePerFrag = Config::Inst()->getFloat("bonusTimePerFrag");

	_bonusTime += bonusTimePerFrag;
}

bool CFGameplayComponent::tryShoot(const Point& whereTo) {

	auto playerPtr = _playerWPtr.lock();

	if (!playerPtr) {
		Log::Inst()->PutErr("CFGameplayComponent::tryShoot error, player not found");
		return false;
	}

	const Size& gameSize = getSize();
	const Point center = gameSize / 2.f;

	const Point from(center.getX(), gameSize.getY() - CfgStatic::playerSize.getY());
	const Point direction(whereTo - from);

	const std::string& playerStateName = playerPtr->GetState();

	if (direction.getY() > 0 || playerStateName != CfgStatic::idleStateName) {
		return false;
	}

	GameObjectPtr bottleObj = std::make_shared<Bullet>(newID());

	bottleObj->SetPosition(from);
	bottleObj->SetDirection(direction);

	addObject(bottleObj, _bulletObjects);

	playerPtr->Shoot();

	return true;
}

bool CFGameplayComponent::initCfgSound(const std::string& cfgTrack, std::string& outputStr) {

	const bool hasValue = Config::Inst()->hasValue(cfgTrack);
	if (!hasValue) {
		return false;
	}

	const std::string& trackStr = Config::Inst()->getString(cfgTrack);

	if (!trackStr.empty()) {		

		bool ok = SoundManager::IsSoundLoaded(trackStr);

		if (!ok) {
			const bool loadedOk = SoundManager::LoadSound(trackStr);
			if (loadedOk) {
				ok = true;
			}
		} 

		if (ok) {
			outputStr = trackStr;
			return true;
		}
	}

	return false;
};

void CFGameplayComponent::initSound() {

	for (auto& s : CfgStatic::boomSounds) {
		SoundManager::LoadSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		SoundManager::LoadSound(s);
	}

	SoundManager::LoadSound(CfgStatic::readySound);
	SoundManager::LoadSound(CfgStatic::throwSound);
	SoundManager::LoadSound(CfgStatic::messageSound);

	initCfgSound("musicTrack", _musicTrack);
	initCfgSound("ambientTrack", _ambientTrack);
}

void CFGameplayComponent::initDialogs() {
	
	_dlgMgr = std::make_shared<DlgManager>();
	_dlgMgr->SetGameServicesPtr(getGameServicesWPtr());

	const auto introDlg = std::make_shared<DialogInfo>(CfgStatic::introDlgName, CfgStatic::introDlgSpr, CfgStatic::introDlgFramesCount);
	const auto outroDlg = std::make_shared<DialogInfo>(CfgStatic::outroDlgName, CfgStatic::outroDlgSpr, CfgStatic::outroDlgFramesCount);

	_dlgMgr->AddDialog(introDlg);
	_dlgMgr->AddDialog(outroDlg);
}

void CFGameplayComponent::Init() {

	GameplayComponent::Init();

	Log::Inst()->PutMessage("CFGameplayComponent::Init");

	initSound();

	const Size& gameSize = getSize();
	const Point center = gameSize / 2.f;

	auto bgObjectPtr = std::make_shared<GameObject>(newID(), CfgStatic::bgName, CfgStatic::bgSprName);
	bgObjectPtr->SetSize(gameSize);
	bgObjectPtr->SetPosition(center);

	addObject(bgObjectPtr, _miscObjects);
	_bgWPtr = bgObjectPtr;

	auto playerPtr = std::make_shared<Player>(newID());
	playerPtr->SetPosition({ gameSize.getX() / 2.f, gameSize.getY() - CfgStatic::playerSize.getY() / 2.f });
	addObject(playerPtr, _miscObjects);

	_playerWPtr = playerPtr;

	initDialogs();
}

void CFGameplayComponent::OnCursorMoved(const Point& pt) {

	Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::OnCursorMoved " + pt.strInt());

	if (getPaused()) {
		return;
	}

	auto playerPtr = _playerWPtr.lock();

	if (playerPtr && !isGameOverAnimStarted()) {
		const bool onLeftSide = pt.getX() < getSize().getX() / 2.f;
		playerPtr->SetMirrorX(onLeftSide);
	}
}

void CFGameplayComponent::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.strInt());

	schedule([this, pt]() {

		const auto advanceDlgFunc = [this]() -> bool {
			const bool advance = _dlgMgr->AdvanceDialog();

			if (advance) {
				SoundManager::PlaySound(CfgStatic::messageSound);
				return true;
			}

			return false;
		};

		if (_state == GameState::INTRO) {

			const bool advance = advanceDlgFunc();
			if (!advance) {
				_dlgMgr->EndDialog();
			}
		}
		else if (_state == GameState::MAIN) {
			tryShoot(pt);
		}
		else if (_state == GameState::OUTRO) {
			advanceDlgFunc();
		}
	});
}

void CFGameplayComponent::SetSkipIntro() {
	
	Log::Inst()->PutMessage("CFGameplayComponent set to skip intro");
	_skipIntro = true;
}
