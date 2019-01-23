#include "Bullet.h"

#include "Bullet.h"
#include "Config.h"

Bullet::Bullet(const IDType id, const GameWPtr game)
	: GameObject(id, CfgStatic::bulletName, CfgStatic::bulletSpr, game) {

	getIdleState()->_particles = "smoke";

	AddAnimation(CfgStatic::bulletSpr, CfgStatic::bulletAnimFramesCount, CfgStatic::bulletAnimFPS);
	AddAnimation(CfgStatic::boomSpr, CfgStatic::boomAnimFramesCount, CfgStatic::boomAnimFPS);

	const float speed      = Utils::rndfMinMax(CfgStatic::bulletSpeedMin,      CfgStatic::bulletSpeedMax);
	const float angleSpeed = Utils::rndfMinMax(CfgStatic::bulletAngleSpeedMin, CfgStatic::bulletAngleSpeedMax);

	SetSpeed(speed);
	SetAngleSpeed(angleSpeed);
}

void Bullet::Boom() {
	
	GameObject::StatePtr deadState = GameObject::State::New(CfgStatic::deadStateName);
	GameObject::StatePtr boomState = GameObject::State::New(CfgStatic::boomStateName);
	
	const float boomDuration = float(CfgStatic::boomAnimFramesCount) / float(CfgStatic::boomAnimFPS);

	boomState->_animation = CfgStatic::boomSpr;
	boomState->_duration = boomDuration;
	boomState->_particles = "smoke"; // though we call StopEmitters below, still need to keep 'smoke' here to let all the puffs disappear naturally. 
	boomState->_sound = Utils::rndStr(CfgStatic::boomSounds);

	const float timeNeedToStopPuffs = CfgStatic::smokeLifetime - boomDuration;

	if (timeNeedToStopPuffs > 0.f) {
		/* that means even after the explosion animation ends, some smoke puffs may still be there, so we're adding
		   one extra service-'state' just to be sure object lives long enough to let all the clouds dissolve in the air.
		*/

		GameObject::StatePtr boomedState = GameObject::State::New(CfgStatic::boomedStateName);

		boomedState->_duration = timeNeedToStopPuffs;
		boomedState->_particles = "smoke"; // still those puffs...
		
		boomState->_nextState = boomedState;
		boomedState->_nextState = deadState;
	}
	else {
		// boom animation seems to be long enough to let all the smoke end its lifecycle.
		boomState->_nextState = deadState;
	}

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
