#include "GameObject.h"

#include "Log.h"
#include "CfgStatic.h"
#include "Animation.h"
#include "EngineComponent.h"
#include "SoundManager.h"
#include "ShaderHelper.h"
#include "Utils.h"

#include <algorithm>

static const std::string emptyStr;

GameObject::GameObject(const IDType id, const std::string& name, const std::string& idleAnim) :_id(id), _name(name), _idleAnimation(idleAnim) {

	if (_idleState == nullptr) {
		_idleState = State::New(CfgStatic::idleStateName);
		_idleState->_animation = _idleAnimation;
	}
}

GameObject::~GameObject() {
	Log::Inst()->PutMessage("~GameObject " + getFullName());
}

void GameObject::SetSize(const Size& sz)        { _size = sz;         }
void GameObject::SetPosition(const Point& pt)   { _position = pt;     }
void GameObject::SetMirrorX(const bool mirrorX) { _mirrorX = mirrorX; }
void GameObject::SetScale(const float scale)    { _scale = scale;     }
void GameObject::SetSpeed(const float s)        { _speed = s;         }
void GameObject::SetAngleSpeed(const float s)   { _angleSpeed = s;    }

void GameObject::SetDirection(const Point& d) {
	_direction = d.normalized();
}

void GameObject::SetEngineComponent(EngineComponentPtr component) {
	if (_engineComponent != nullptr) {
		Log::Inst()->PutErr("GameObject::SetEngineComponent error, engineComponent is already set");
	}

	_engineComponent = component;
}

EngineComponentPtr GameObject::GetEngineComponent() const { return _engineComponent; };

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

AnimationPtr GameObject::getAnimation(const std::string& animName, const bool onlyTry) const {

	const auto& animIt = _animations.find(animName);
	if (animIt == _animations.end()) {

		if (!onlyTry) {
			Log::Inst()->PutErr("GameObject::GetAnimation error, not found animation " + animName);
		}
		return AnimationPtr();
	}
	return animIt->second;
}

void GameObject::onStateUpdate(const StatePtr& prevState) {

	const auto soundFunc = [](const std::string& soundName) {
		if (!soundName.empty()) {
			SoundManager::PlaySound(soundName);
		}
	};

	if (prevState) {
		soundFunc(prevState->_soundEnd);
	}

	soundFunc(_state->_sound);
	_latestAnimation.reset();
}

void GameObject::ChangeState(const StatePtr& newState) {

	Log::Inst()->PutMessage("object " + getFullName() + " will change state from " + (_state ? _state->_name : "(none)") + " to " + newState->_name);

	const StatePtr prevState = _state;
	_state = newState;
	_state->_startTime = _gameSimulationTime;
	onStateUpdate(prevState);
}

const std::string& GameObject::GetAnimation() const {

	if (!_state) {
		return emptyStr;
	}

	return _state->_animation;
}

const std::string& GameObject::GetState() const {

	if (!_state) {
		return emptyStr;
	}

	return _state->_name;
}

bool GameObject::updateState() {

	const float currTime = _gameSimulationTime;
	bool changed = false;

	if (!_state) {
		
		const AnimationPtr idleAnimPtr = getAnimation(_idleAnimation, true);
		if (!idleAnimPtr) {
			AddAnimation(_idleAnimation);
		}
		
		ChangeState(_idleState);
		changed = true;
	}

	float duration = _state->_duration;

	if (!_state->_animation.empty()) {

		if (duration == basedOnAnimation) { 
			const AnimationPtr animation = getAnimation(_state->_animation);

			if (animation == nullptr) {
				Log::Inst()->PutErr("GameObject::updateState error, state " + _state->_name + " animation " + _state->_animation);
				return changed;
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
		const StatePtr newState = _state->_nextState ? _state->_nextState : _idleState;
		ChangeState(newState);
		changed = true;
	}

	return changed;
}

float GameObject::GetEffectCoeff(const float maxDuration, const float minVal, const float maxVal) const {

	float coeff = minVal;
	const auto& state = getState();

	if (!state) {
		Log::Inst()->PutErr("GameObject::GetEffectCoeff error, no valid state. obj=" + getFullName());
		return coeff;
	}
	
	const float time = getGameSimulationTime();
	const float dtFromAnimBegin = Utils::dt(time, state->_startTime);

	const float progress = std::min<float>(1.f, dtFromAnimBegin / maxDuration);
	const float dVal = maxVal - minVal;

	if (dVal < 0) {
		Log::Inst()->PutErr("GameObject::GetEffectCoeff error, wrong min/max values. obj=" + getFullName());
		return coeff;
	}

	coeff += progress * dVal;
	return coeff;
}

const std::string& GameObject::getParticlesName() const {
	
	if (!_state) {
		return emptyStr;
	}

	return _state->_particles;
}

const std::string& GameObject::getShaderName() const {

	if (!_state){
		return emptyStr;
	}

	return _state->_shader;
}

ShaderPtr GameObject::GetShader() const {

	ShaderPtr shader;
	if (_state && !_state->_shader.empty() && _engineComponent) {
		
		shader = _engineComponent->GetShader();
		ConfigureShader(_state->_shader, this);
		//todo: how to avoid calling this every frame?
	}

	return shader;
}

const std::string& GameObject::GetCurrentSpriteName() const {

	if (_state == nullptr || _state->_animation.empty()) {
		return emptyStr;
	}

	const AnimationPtr lastAnimPtr = _latestAnimation.lock();
	AnimationPtr animation;

	if (lastAnimPtr) {
		animation = lastAnimPtr;
	} else {
		animation = getAnimation(_state->_animation);
		_latestAnimation = animation;
	}

	if (animation == nullptr) {
		Log::Inst()->PutErr("GameObject::GetCurrentSpriteNam error, animation broken :( " + getFullName());
		return emptyStr;
	}

	const float time = _gameSimulationTime;
	const float dtFromAnimBegin = Utils::dt(time, _state->_startTime);

	const std::string& frameName = animation->GetFrameName(dtFromAnimBegin);
	return frameName;
}

Point GameObject::GetEmitterPosition() const {
	return _position;
}

std::string GameObject::getFullName() const {
	return _name + std::to_string(getId());
}

void GameObject::Update(const float dt, const float gameTime) {
	
	_gameSimulationTime = gameTime;

	if (_direction.isEmpty()) {
		Log::Inst()->PutErr("GameObject::update error, invalid direction " + getFullName());
	}

	if (_size.isEmpty() && _engineComponent != nullptr) {
		const Size& sizeFromEngine = _engineComponent->GetSpriteSize();
		Log::Inst()->PutMessage("GameObject " + getFullName() + " size not set, will get it from engine: " + sizeFromEngine.strInt() + " but that's not cool, you know");
		_size = sizeFromEngine;
	}

	_speed += _acceleration * dt;
	_position += _direction * _speed * dt;

	const float angleFullRound = 360;
	_rotation += fmod(_angleSpeed * dt, angleFullRound);

	updateState();

	if (_engineComponent) {
		_engineComponent->Update(dt);
	}
}

bool GameObject::RequestKill(const std::string& reason) {
	
	// by default, any object can be destroyed without any waiting
	return true;
}