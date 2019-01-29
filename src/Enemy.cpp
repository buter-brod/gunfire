#include "Enemy.h"

#include "Config.h"
#include "ResourceManager.h"
#include "Game.h"

Enemy::Enemy(const IDType id, const GameWPtr game)
	: GameObject(id, CfgStatic::enemyName, CfgStatic::enemySpr, game) {

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

float Enemy::GetPixelizeCoeff() const {

	float coeff = 0;

	const std::string& shaderName = _state->_shader;
	if (shaderName == CfgStatic::pixelizeShader) {
		const float time = _gameSimulationTime;
		const float dtFromAnimBegin = Utils::dt(time, _state->_startTime);

		coeff = std::min(CfgStatic::pixelizeCoeffMax, CfgStatic::pixelizeSpeed * dtFromAnimBegin + 1.f);
	}

	return coeff;
}

ShaderPtr Enemy::GetShader() {

	ShaderPtr shader = GameObject::GetShader();
	const float pCoeff = GetPixelizeCoeff();

	if (pCoeff > 0.f) {
		if (shader) {
			const auto& bigRect = getSprite()->getSpr()->getTexture()->getSize();
			const auto& smallRect = getSprite()->getSpr()->getTextureRect();

			float texRect[4] = {
				float(smallRect.left),
				float(bigRect.y - smallRect.top),
				float(smallRect.width),
				float(smallRect.height) };

			shader->SetUniform("bigRectSize", (float)bigRect.x, (float)bigRect.y);
			shader->SetUniform("smallRect", texRect[0], texRect[1], texRect[2], texRect[3]);
			shader->SetUniform("coeff", pCoeff);
		}
		else {
			Log::Inst()->PutErr("Enemy::GetShader error, shader not found " + _state->_shader + ", but pixelaze coeff " + std::to_string(pCoeff));
		}
	}

	return shader;
}

void Enemy::Update(float dt) {

	GameObject::Update(dt);

	const float x = _position.getX();
	const float xMax = CfgStatic::gameSize.getX();

	const bool needTurn = _direction.getY() == 0.f && (
		(_direction.getX() < 0.f && x < 0.f) ||
		(_direction.getX() > 0.f && x > xMax));

	if (needTurn) {
		_direction.X() = -_direction.getX();
		_mirrorX = !_mirrorX;
	}

	if (_particles) {

		if (GetState() == CfgStatic::enemyDStateName) {

			const float currTime = _gameSimulationTime;
			const float stateElapsed = Utils::dt(currTime, _state->_startTime);
			if (stateElapsed > CfgStatic::boomLifetime) {
				_particles->StopEmitters();
			}
		}
	}
}

Point Enemy::getEmitterPosition() {

	if (GetState() == CfgStatic::enemyDStateName) {
		return _boomPosition;
	}
	else {
		return GameObject::getEmitterPosition();
	}
}

void Enemy::Boom(const Point& bulletPos) {

	Point enemyDirection = GetDirection().normalized();
	enemyDirection.Y() = -1.f;
	SetDirection(enemyDirection);

	SetSpeed(CfgStatic::boomAcceleration);
	SetAngleSpeed((Utils::rndYesNo() ? 1.f : -1.f) *  CfgStatic::boomAngleSpeed);

	GameObject::StatePtr enemyDeadState = GameObject::State::New(CfgStatic::enemyDStateName);
	enemyDeadState->_animation = getIdleState()->_animation;
	enemyDeadState->_shader = CfgStatic::pixelizeShader;
	enemyDeadState->_particles = "boom";

	ChangeState(enemyDeadState);

	_boomPosition = bulletPos;
}