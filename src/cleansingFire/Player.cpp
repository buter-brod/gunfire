#include "Player.h"
#include "Config.h"

Player::Player(const IDType id)
: GameObject(id, CfgStatic::playerName, CfgStatic::playerSpr) {

	SetSize(CfgStatic::playerSize);

	AddAnimation(CfgStatic::playerFireSpr);
	AddAnimation(CfgStatic::playerCooldownSpr);
}

void Player::Shoot() {
	GameObject::StatePtr shootState = GameObject::State::New(CfgStatic::shootingStateName);
	shootState->_animation = CfgStatic::playerFireSpr;
	shootState->_duration = Config::Inst()->getFloat("throwDuration");
	shootState->_sound = CfgStatic::throwSound;

	GameObject::StatePtr cooldownState = GameObject::State::New(CfgStatic::cooldownStateName);
	cooldownState->_animation = CfgStatic::playerCooldownSpr;
	cooldownState->_duration = Config::Inst()->getFloat("throwCooldown");
	cooldownState->_soundEnd = CfgStatic::readySound;

	shootState->_nextState = cooldownState;

	ChangeState(shootState);
}
