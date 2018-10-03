#include "Bullet.h"

#include "Bullet.h"
#include "Config.h"

Bullet::Bullet(const IDType id)
	: GameObject(id, CfgStatic::bulletName, CfgStatic::bulletSpr) {

	getIdleState()->_particles = "smoke";

	AddAnimation(CfgStatic::bulletSpr, CfgStatic::bulletAnimFramesCount, CfgStatic::bulletAnimFPS);
	AddAnimation(CfgStatic::boomSpr, CfgStatic::boomAnimFramesCount, CfgStatic::boomAnimFPS);

	const float speed      = Utils::rndfMinMax(CfgStatic::bulletSpeedMin,      CfgStatic::bulletSpeedMax);
	const float angleSpeed = Utils::rndfMinMax(CfgStatic::bulletAngleSpeedMin, CfgStatic::bulletAngleSpeedMax);

	SetSpeed(speed);
	SetAngleSpeed(angleSpeed);
}

void Bullet::Boom() {
	
	GameObject::StatePtr boomState = GameObject::State::New(CfgStatic::boomStateName);

	boomState->_duration = CfgStatic::smokeLifetime;
	boomState->_particles = "smoke";
	boomState->_sound = CfgStatic::boomSounds[Utils::rnd0xi((unsigned int)CfgStatic::boomSounds.size())];

	GameObject::StatePtr deadState = GameObject::State::New(CfgStatic::deadStateName);
	boomState->_nextState = deadState;

	ChangeState(boomState);
	SetSpeed(0.f);
	SetAngleSpeed(0.f);

	if (_particles) {
		_particles->StopEmitters();
	}
}

Point Bullet::getEmitterPosition() {
	
	Point emitterPos = _position;

	// smoke should go from the bottle neck
	emitterPos.Y() -= GetSize().getY() / 2.f;
	emitterPos.rotate(_position, _rotation);

	return emitterPos;
}
