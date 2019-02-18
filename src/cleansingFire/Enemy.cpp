#include "Enemy.h"

#include "CFCfgStatic.h"
#include "Config.h"
#include "EngineComponent.h"

Enemy::Enemy(const IDType id)
	: GameObject(id, CfgStatic::enemyName, CfgStatic::enemySpr) {

	SetSize(CfgStatic::enemySize);

	AddAnimation(CfgStatic::enemySpr, CfgStatic::enemyAnimFramesCount, CfgStatic::enemyAnimFPS);

	const unsigned int enemyCount = Config::Inst()->getInt("enemyCount");
	const bool spawnSound = Utils::rnd0xi(enemyCount) == 0;
	if (spawnSound) {
		getIdleState()->_sound = Utils::rndStr(CfgStatic::enemySounds);
	}

	const float enemySpeedMin = Config::Inst()->getFloat("enemySpeedMin");
	const float enemySpeedMax = Config::Inst()->getFloat("enemySpeedMax");

	const float speed = Utils::rndfMinMax(enemySpeedMin, enemySpeedMax);

	SetSpeed(speed);
}

void Enemy::Update(const float dt, const float gameTime) {

	GameObject::Update(dt, gameTime);

	const Point& pos = GetPosition();
	Point dir = GetDirection();

	const float x = pos.getX();
	const float xMax = CfgStatic::gameSize.getX();

	const bool inIdleState = (GetState() == CfgStatic::idleStateName);

	const bool needTurn = inIdleState && (
		(dir.getX() < 0.f && x < 0.f) ||
		(dir.getX() > 0.f && x > xMax));

	if (needTurn) {
		dir.X() = -dir.getX();
		SetDirection(dir);

		SetMirrorX(!GetMirrorX());
	}

	if (GetState() == CfgStatic::dyingStateName) {

		const float currTime = getGameSimulationTime();
		const float stateElapsed = Utils::dt(currTime, getState()->_startTime);
		if (stateElapsed > CfgStatic::boomLifetime) {

			auto engineComponent = GetEngineComponent();
			if (engineComponent) {
				engineComponent->StopEmitters();
			}
		}
	}
}

Point Enemy::GetEmitterPosition() const {

	if (GetState() == CfgStatic::dyingStateName) {
		return _boomPosition;
	}
	else {
		return GameObject::GetEmitterPosition();
	}
}

void Enemy::Boom(const Point& bulletPos) {

	Point enemyDirection = GetDirection().normalized();
	enemyDirection.Y() = -1.f;
	SetDirection(enemyDirection);

	SetSpeed(CfgStatic::boomAcceleration);
	SetAngleSpeed((Utils::rndYesNo() ? 1.f : -1.f) *  CfgStatic::boomAngleSpeed);

	GameObject::StatePtr enemyDyingState = GameObject::State::New(CfgStatic::dyingStateName);
	enemyDyingState->_animation = getIdleState()->_animation;
	enemyDyingState->_shader = CfgStatic::pixelizeShader;
	enemyDyingState->_particles = "boom";

	ChangeState(enemyDyingState);

	_boomPosition = bulletPos;
}