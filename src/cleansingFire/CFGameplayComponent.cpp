#include "CFGameplayComponent.h"
#include "Log.h"
#include "Config.h"
#include "CfgStatic.h"
#include "CFCfgStatic.h"
#include "SoundManager.h"

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

	if (!_dlgObjects.empty()) {
		return { &_dlgObjects };
	}

	return { &_miscObjects, &_enemyObjects, &_bulletObjects };
}

bool CFGameplayComponent::Update(const float dt) {

	_timeRemain = getTimeRemain();

	const bool introDlgNow = checkIntroDlg();
	const bool outroDlgNow = checkOutroDlg();

	const bool dlgActive = introDlgNow || outroDlgNow;

	if (!dlgActive) {
		const bool removed = removeDlg();

		if (removed) {
			SoundManager::PlaySound(_musicTrack, true);
			SoundManager::PlaySound(CfgStatic::ambientSound, true);
		}
	}

	const float t = dlgActive ? 0.f : dt;

	if (t > 0.f) {
		if (_timeRemain > 0.f) {

			if (_shootRequest.requested) {
				tryShoot(_shootRequest.targetPt);
				_shootRequest = { false, Point() };
			}

			checkSpawn();
			checkCollisions();
		}
		else {

			if (!isGameOverAnimStarted()) {
				startGameOverAnim();
			}
		}
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
	const Point center = gameSize / 2.f;

	const bool outOfBounds =
		pos.getX() < -sz.getX() ||
		pos.getY() < -sz.getY() ||

		pos.getX() > gameSize.getX() + sz.getX() ||
		pos.getY() > gameSize.getY() + sz.getY();

	if (outOfBounds) {
		// not visible by game bounds

		const bool notMoving = objPtr->GetSpeed() == 0.f;

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

void CFGameplayComponent::onCollision(GameObjectPtr bullet, GameObjectPtr enemy) {

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

void CFGameplayComponent::initSound() {

	for (auto& s : CfgStatic::boomSounds) {
		SoundManager::LoadSound(s);
	}

	for (auto& s : CfgStatic::enemySounds) {
		SoundManager::LoadSound(s);
	}

	SoundManager::LoadSound(CfgStatic::readySound);
	SoundManager::LoadSound(CfgStatic::throwSound);

	const auto initCfgSound = [](const std::string& cfgOption, const std::string& cfgTrack, std::string& outputStr) {

		const bool trackInCfg = Config::Inst()->hasValue(cfgTrack);
		const bool enabledCfg = !Config::Inst()->hasValue(cfgOption) || Config::Inst()->getInt(cfgOption) == 0;

		if (trackInCfg != enabledCfg) {
			Log::Inst()->PutErr("CFGameplayComponent::initSound initCfgSound error, check music/ambient settings");
		}
		else if (trackInCfg && enabledCfg) {

			const std::string& trackStr = Config::Inst()->getString(cfgTrack);
			const bool loadedOk = SoundManager::LoadSound(trackStr);

			if (loadedOk) {
				outputStr = trackStr;
			}
		}
	};
	
	initCfgSound("noMusic", "musicTrack", _musicTrack);
	initCfgSound("noAmbient", "ambientTrack", _ambientTrack);
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

void CFGameplayComponent::tryDlgAdvance() {
	
	const auto tryRemoveTxt = [this]() {
		const auto dlgTxtObj = _dialogTxt.lock();

		if (dlgTxtObj) {
			removeObject(dlgTxtObj, _dlgObjects);
			_dialogTxt.reset();
		}
	};

	if (IsIntroDialogActive()) {
		++_introDlgState;
		tryRemoveTxt();
	}
	else if (IsOutroDialogActive() && _outroDlgState < CfgStatic::outroDlgFramesCount - 1) {
		++_outroDlgState;
		tryRemoveTxt();
	}	
}

void CFGameplayComponent::OnCursorClicked(const Point& pt) {
	Log::Inst()->PutMessage("Game::OnCursorClicked " + pt.strInt());

	if (getPaused()) {
		return;
	}

	tryDlgAdvance();

	_shootRequest = { true, pt };
}

void CFGameplayComponent::SetSkipIntro() {
	_introDlgState = -1;
}

bool CFGameplayComponent::checkIntroDlg() {

	const bool shouldBeIntro = IsIntroDialogActive();

	if (shouldBeIntro) {
		addDlg(CfgStatic::introDlgSpr, _introDlgState);
		return true;
	}

	return false;
}

bool CFGameplayComponent::checkOutroDlg() {

	const bool shouldBeOutro = IsOutroDialogActive();

	if (shouldBeOutro) {

		if (_outroDlgState < CfgStatic::outroDlgFramesCount) {
			addDlg(CfgStatic::outroDlgSpr, _outroDlgState);
		}

		return true;
	}

	return false;
}

bool CFGameplayComponent::addDlg(const std::string& sprName, const unsigned int number) {

	const auto addToCenter = [this](const GameObjectPtr obj) {
		const Point center = CfgStatic::gameSize / 2.f;
		obj->SetPosition(center);
		addObject(obj, _dlgObjects);
	};

	if (!_dialogBg.lock()) {

		const auto dlgBgObj = std::make_shared<GameObject>(newID(), CfgStatic::bgDlgName, CfgStatic::bgDlgSpr);
		dlgBgObj->SetSize(CfgStatic::gameSize);
		addToCenter(dlgBgObj);
		_dialogBg = dlgBgObj;
	}

	if (!_dialogWndBg.lock()) {

		const auto dlgWndBgObj = std::make_shared<GameObject>(newID(), CfgStatic::bgWndDlgName, CfgStatic::bgWndDlgSpr);
		dlgWndBgObj->SetSize(CfgStatic::bgDlgSize);
		addToCenter(dlgWndBgObj);
		_dialogWndBg = dlgWndBgObj;
	}

	if (!_dialogTxt.lock()) {

		std::string introDlgSprName = sprName;

		const auto dotPos = introDlgSprName.find('.'); // if names contain file extension, put frame number at the end of the name-part.
		introDlgSprName.insert(dotPos, std::to_string(number));

		const auto dlgTxtObj = std::make_shared<GameObject>(newID(), CfgStatic::textDlgName, introDlgSprName);
		addToCenter(dlgTxtObj);
		_dialogTxt = dlgTxtObj;
	}

	return true;
}

bool CFGameplayComponent::removeDlg() {

	const auto removeFunc = [this](GameObjectWPtr& wPtr) -> bool {
		const auto obj = wPtr.lock();
		if (obj) {
			removeObject(obj, _dlgObjects);
			wPtr.reset();
			return true;
		}

		return false;
	};

	const bool wndRemoved = removeFunc(_dialogWndBg);
	const bool txtRemoved = removeFunc(_dialogTxt);
	const bool bgRemoved  = removeFunc(_dialogBg);

	const bool removedAnything = wndRemoved || txtRemoved || bgRemoved;

	return removedAnything;
}

bool CFGameplayComponent::IsIntroDialogActive() const {
	return _introDlgState >= 0 && _introDlgState < (int)CfgStatic::introDlgFramesCount;
}

bool CFGameplayComponent::IsOutroDialogActive() const {

	return _timeRemain <= 0.f &&
		_outroDlgState >= 0 &&
		!_playerWPtr.lock();
}