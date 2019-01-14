#include "GameObject.h"
#include "Log.h"
#include "Config.h"
#include "ResourceManager.h"
#include "Game.h"

GameObject::GameObject(const IDType id, const std::string& name, const std::string& idleAnim, const GameWPtr game) :_id(id), _name(name), _idleAnimation(idleAnim), _gamePtr(game) {

	if (!_gamePtr.lock()) {
		Log::Inst()->PutErr("GameObject::GameObject " + Utils::toString(id) + " trying to create with empty game");
	}

	if (!_idleState) {
		_idleState = State::New(CfgStatic::idleStateName);
		_idleState->_animation = _idleAnimation;
	}
}

GameObject::~GameObject() {
	Log::Inst()->PutMessage("~GameObject " + _name + " id " + std::to_string(_id));
}

void GameObject::SetPosition(const Point& pt) {
	_position = pt;
}

void GameObject::SetSize(const Size& sz) {
	_size = sz;
}

void GameObject::SetMirrorX(const bool mirrorX) {
	_mirrorX = mirrorX;
}

void GameObject::SetScale(const float scale) {
	_scale = scale;
}

void GameObject::SetDirection(const Point& d) {
	_direction = d;
}

void GameObject::SetSpeed(const float s) {
	_speed = s;
}

void GameObject::SetAngleSpeed(const float s) {
	_angleSpeed = s;
}

AnimationPtr GameObject::AddAnimation(const std::string& name) {
	return AddAnimation(name, 1, 0);
}

AnimationPtr GameObject::AddAnimation(const std::string& name, const unsigned int framesCount, const unsigned int fps) {

	AnimationPtr animPtr;
	const auto& animIt = _animations.find(name);

	if (animIt != _animations.end()) {

		Log::Inst()->PutMessage("GameObject::AddAnimation " + name + " already added, no action needed");
		animPtr = animIt->second;
	}
	else {
		animPtr = std::make_shared<Animation>(name, framesCount, fps);
		_animations[name] = animPtr;
	}
	
	return animPtr;
}

AnimationPtr GameObject::GetAnimation(const std::string& animName, const bool onlyTry) {

	const auto& animIt = _animations.find(animName);

	if (animIt == _animations.end()) {

		if (!onlyTry) {
			Log::Inst()->PutErr("GameObject::GetAnimation error, not found animation " + animName);
		}
		return AnimationPtr();
	}
	return animIt->second;
}

void GameObject::updateScale() {

	if (_spritePtr == nullptr) {
		return;
	}

	const auto& tRect = _spritePtr->getSpr()->getTextureRect();

	if (_size.len() == 0.f) {
		_size = { float(tRect.width), float(tRect.height) }; // will use texture size
	}

	_spritePtr->getSpr()->setScale(
		_size.getX() / tRect.width * _scale * (_mirrorX ? -1.f : 1.f),
		_size.getY() / tRect.height * _scale);

	const Size halfSize(float(tRect.width) / 2.f, float(tRect.height) / 2.f);
	_spritePtr->getSpr()->setOrigin(halfSize.getX(), halfSize.getY());
}

void GameObject::onStateUpdate(const StatePtr prevState) {

	auto soundFunc = [](const std::string& soundName) {
		if (!soundName.empty()) {
			SoundPtr soundPtr = ResourceManager::Inst()->GetSound(soundName);

			if (!soundPtr) {
				Log::Inst()->PutErr("GameObject::onStateUpdate unable to play sound " + soundName);
			}
			else {
				soundPtr->Play();
			}
		}
	};

	if (prevState) {
		soundFunc(prevState->_soundEnd);
	}

	soundFunc(_state->_sound);

	if (!prevState || prevState->_particles != _state->_particles) {
		_particles = Particles::Build(_state->_particles);
	}
}

void GameObject::ChangeState(StatePtr newState) {

	Log::Inst()->PutMessage("object " + _name + " id " + std::to_string(_id) + " will change state from " + (_state ? _state->_name : "(none)") + " to " + newState->_name);

	StatePtr prevState = _state;
	_state = newState;
	_state->_startTime = _gameSimulationTime;
	onStateUpdate(prevState);
}

const std::string GameObject::GetState() const {
	return _state ? _state->_name : "";
}

void GameObject::updateState() {

	const float currTime = _gameSimulationTime;

	if (!_state) {
		
		AnimationPtr idleAnimPtr = GetAnimation(_idleAnimation, true);
		if (!idleAnimPtr) {
			AddAnimation(_idleAnimation);
		}
		
		ChangeState(_idleState);
	}

	float duration = _state->_duration;

	if (!_state->_animation.empty()) {

		if (duration == basedOnAnimation) {
			AnimationPtr animation = GetAnimation(_state->_animation);

			if (animation == nullptr) {
				Log::Inst()->PutErr("GameObject::updateState error, state " + _state->_name + " animation " + _state->_animation);
				return;
			}

			const unsigned int animFPS = animation->GetFPS();

			if (animFPS == 0) {
				Log::Inst()->PutErr("GameObject::updateState error, state " + _state->_name + " duration is based on animation, but FPS == 0");
				duration = 0.f;
			}
			else {
				duration = float(animation->GetSize()) / animFPS;
			}
		}
	}

	const float stateElapsed = Utils::dt(currTime, _state->_startTime);

	if (duration > 0.f && stateElapsed > duration) {
		StatePtr newState = _state->_nextState ? _state->_nextState : _idleState;
		ChangeState(newState);
	}
}

ParticlesPtr GameObject::getParticles() {

	return _particles;
}

ShaderPtr GameObject::GetShader() {

	const std::string& shaderName = _state->_shader;
	ShaderPtr shader;

	if (!shaderName.empty()) {
		
		shader = ResourceManager::Inst()->GetShader(shaderName);

		if (!shader) {
			Log::Inst()->PutErr("GameObject::GetShader error: unable to get " + shaderName);
		}
	}
	
	return shader;
}

void GameObject::updateAnimations() {
		
	if (_state->_animation.empty()) {

		if (_spritePtr) {
			_spritePtr = nullptr;
		}
		return;
	}

	AnimationPtr animation = GetAnimation(_state->_animation);

	if (animation == nullptr) {
		Log::Inst()->PutErr("GameObject::update error, animation broken :(");
		return;
	}

	const float time = _gameSimulationTime;
	const float dtFromAnimBegin = Utils::dt(time, _state->_startTime);
	const auto& textureRect = animation->GetTexRectFor(dtFromAnimBegin);
	const auto& texturePtr = textureRect.texturePtr.lock();

	if (texturePtr == nullptr) {
		Log::Inst()->PutErr("GameObject::update error, animation texture not found? " + animation->getName());
		return;
	}

	if (_spritePtr == nullptr) {
		_spritePtr = std::make_shared<Sprite>(*texturePtr, textureRect.rect);
	}
	else {
		_spritePtr->getSpr()->setTexture(*texturePtr->getTex());
		_spritePtr->getSpr()->setTextureRect(textureRect.rect);
	}
}

Point GameObject::getEmitterPosition() {
	return _position;
}

void GameObject::Update(float dt) {

	auto gameLock = _gamePtr.lock();

	if (!gameLock) {
		Log::Inst()->PutMessage("GameObject::Update error, gameLock invalid");
		return;
	}

	_gameSimulationTime = gameLock->GetSimulationTime();

	if (_direction.len() == 0) {
		Log::Inst()->PutErr("GameObject::update error, invalid direction " + std::to_string(getId()));
	}
	
	_speed += _acceleration * dt;
	_position += _direction.normalized() * _speed * dt;

	const float angleFullRound = 360;
	_rotation += fmod(_angleSpeed * dt, angleFullRound);

	updateState();
	updateAnimations();
	updateScale();

	if (_spritePtr) {
		_spritePtr->getSpr()->setRotation(_rotation);
		_spritePtr->getSpr()->setPosition(_position.getX(), _position.getY());
	}

	if (_particles) {
		const Point emitterPos = getEmitterPosition();
		_particles->Update(dt, emitterPos);
	}
}