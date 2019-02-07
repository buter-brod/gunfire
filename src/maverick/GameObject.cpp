#include "GameObject.h"

#include "Log.h"
#include "Config.h"
#include "Animation.h"
#include "EngineComponent.h"
#include "SoundManager.h"
#include "ShaderHelper.h"

#include <algorithm>

GameObject::GameObject(const IDType id, const std::string& name, const std::string& idleAnim) :_id(id), _name(name), _idleAnimation(idleAnim) {

	if (!_idleState) {
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
void GameObject::SetDirection(const Point& d)   { _direction = d;     }
void GameObject::SetSpeed(const float s)        { _speed = s;         }
void GameObject::SetAngleSpeed(const float s)   { _angleSpeed = s;    }

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

void GameObject::onStateUpdate(const StatePtr prevState) {

	auto soundFunc = [](const std::string& soundName) {
		if (!soundName.empty()) {
			SoundManager::PlaySound(soundName);
		}
	};

	if (prevState) {
		soundFunc(prevState->_soundEnd);
	}

	soundFunc(_state->_sound);
}

void GameObject::ChangeState(StatePtr newState) {

	Log::Inst()->PutMessage("object " + getFullName() + " will change state from " + (_state ? _state->_name : "(none)") + " to " + newState->_name);

	StatePtr prevState = _state;
	_state = newState;
	_state->_startTime = _gameSimulationTime;
	onStateUpdate(prevState);
}

const std::string GameObject::GetAnimation() const {
	return _state ? _state->_animation : "";
}

const std::string GameObject::GetState() const {
	return _state ? _state->_name : "";
}

void GameObject::updateState() {

	const float currTime = _gameSimulationTime;

	if (!_state) {
		
		AnimationPtr idleAnimPtr = getAnimation(_idleAnimation, true);
		if (!idleAnimPtr) {
			AddAnimation(_idleAnimation);
		}
		
		ChangeState(_idleState);
	}

	float duration = _state->_duration;

	if (!_state->_animation.empty()) {

		if (duration == basedOnAnimation) {
			AnimationPtr animation = getAnimation(_state->_animation);

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

std::string GameObject::getParticlesName() const {
	std::string pName;

	if (_state) {
		pName = _state->_particles;
	}

	return pName;
}

std::string GameObject::getShaderName() const {

	std::string shName;

	if (_state) {
		shName = _state->_shader;
	}

	return shName;
}

ShaderPtr GameObject::GetShader() {

	ShaderPtr shader;

	if (_engineComponent) {
		shader = _engineComponent->GetShader();

		if (_state) {
			ConfigureShader(_state->_shader, this);
		}	
	}

	return shader;
}

std::string GameObject::GetCurrentSpriteName() const {

	if (_state == nullptr || _state->_animation.empty()) {
		return "";
	}
	
	const AnimationPtr animation = getAnimation(_state->_animation);
	if (animation == nullptr) {
		Log::Inst()->PutErr("GameObject::GetCurrentSpriteNam error, animation broken :( " + getFullName());
		return "";
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

void GameObject::Update(float dt, const float gameTime) {
	
	_gameSimulationTime = gameTime;

	if (_direction.len() == 0) {
		Log::Inst()->PutErr("GameObject::update error, invalid direction " + getFullName());
	}

	if (_size.isEmpty() && _engineComponent != nullptr) {
		const Size& sizeFromEngine = _engineComponent->GetSpriteSize();
		Log::Inst()->PutMessage("GameObject " + getFullName() + " size not set, will get it from engine: " + sizeFromEngine.strInt() + " but that's not cool, you know");
		_size = sizeFromEngine;
	}

	_speed += _acceleration * dt;
	_position += _direction.normalized() * _speed * dt;

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