#include "Enemy.h"

#include "Config.h"
#include "Log.h"
#include "EngineComponent.h"
#include "Shader.h"

#include <algorithm>

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

float Enemy::GetPixelizeCoeff() const {

	float coeff = 0.f;
	const auto& state = getState();

	const std::string& shaderName = state->_shader;
	if (shaderName == CfgStatic::pixelizeShader) {
		const float time = getGameSimulationTime();
		const float dtFromAnimBegin = Utils::dt(time, state->_startTime);

		coeff = std::min(CfgStatic::pixelizeCoeffMax, CfgStatic::pixelizeSpeed * dtFromAnimBegin + 1.f);
	}

	return coeff;
}

ShaderPtr Enemy::GetShader() {

	const float pCoeff = GetPixelizeCoeff();
	ShaderPtr shader = GameObject::GetShader();

	if (pCoeff > 0.f) {
		if (shader) {

			auto engineComp = GetEngineComponent();

			if (!engineComp) {
				Log::Inst()->PutErr("Enemy::GetShader error for " + getFullName() + ", engineComp not found");
				return shader;
			}

			const Size& bigSize = engineComp->GetTextureSize();
			const Rect& smallRect = engineComp->GetSpriteRect();

			float texRect[4] = {
				smallRect._origin.getX(),
				bigSize.getY() - smallRect._origin.getY(),
				smallRect._size.getX(),
				smallRect._size.getY() };

			shader->SetUniform("bigRectSize", bigSize.getX(), bigSize.getY());
			shader->SetUniform("smallRect", texRect[0], texRect[1], texRect[2], texRect[3]);
			shader->SetUniform("coeff", pCoeff);
		}
		else {
			Log::Inst()->PutErr("Enemy::GetShader error for " + getFullName() + ", shader not found " + getShaderName() + ", but pixelaze coeff " + std::to_string(pCoeff));
		}
	}

	return shader;
}

void Enemy::Update(const float dt, const float gameTime) {

	GameObject::Update(dt, gameTime);

	const Point& pos = GetPosition();
	Point dir = GetDirection();

	const float x = pos.getX();
	const float xMax = CfgStatic::gameSize.getX();

	const bool needTurn = dir.getY() == 0.f && (
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