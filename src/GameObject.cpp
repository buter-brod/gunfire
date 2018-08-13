#include "GameObject.h"
#include "Log.h"
#include "Config.h"
#include "ResourceManager.h"

GameObject::GameObject(const IDType id, const std::string& name, const std::string& idleAnim) :_id(id), _name(name), _idleAnimation(idleAnim) {

	if (!_idleState) {
		_idleState = std::make_shared<State>(Config::idleStateName);
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

void GameObject::onStateUpdate() {
}

void GameObject::ChangeState(StatePtr newState) {

	Log::Inst()->PutMessage("object " + _name + " id " + std::to_string(_id) + " will change state from " + (_state ? _state->_name : "-") + " to " + newState->_name);

	auto soundFunc = [](const std::string& soundName) {
		if (!soundName.empty()) {
			SoundPtr soundPtr = ResourceManager::Inst()->GetSound(soundName);

			if (!soundPtr) {
				Log::Inst()->PutErr("GameObject::onStateUpdate unable to play sound " + soundName);
			}
			else {
				soundPtr->get().play();
			}
		}
	};

	if (_state) {
		soundFunc(_state->_soundEnd);
	}
	
	soundFunc(newState->_sound);

	_state = newState;
	_state->_startTime = Utils::getTime();
	onStateUpdate();
}

const std::string GameObject::GetState() const {
	return _state ? _state->_name : "";
}

void GameObject::updateState() {

	const time_us currTime = Utils::getTime();

	if (!_state) {
		
		AnimationPtr idleAnimPtr = GetAnimation(_idleAnimation, true);
		if (!idleAnimPtr) {
			AddAnimation(_idleAnimation);
		}
		
		ChangeState(_idleState);
	}

	const float stateElapsed = Utils::dt(currTime, _state->_startTime);
	float duration = _state->_duration;

	if (!_state->_animation.empty()) {
		AnimationPtr animation = GetAnimation(_state->_animation);

		if (animation == nullptr) {
			Log::Inst()->PutErr("GameObject::updateState error, state " + _state->_name + " animation " + _state->_animation);
			return;
		}

		if (duration == basedOnAnimation) {
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

	if (duration > 0.f && stateElapsed > duration) {
		StatePtr newState = _state->_nextState ? _state->_nextState : _idleState;
		ChangeState(newState);
	}
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

	const time_us time = Utils::getTime();
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

void GameObject::Update(float dt) {

	if (_direction.len() == 0) {
		Log::Inst()->PutErr("GameObject::update error, invalid direction " + std::to_string(getId()));
	}

	if (_name == Config::enemyName) {
		// handle turn back on edges

		const float x = _position.getX();
		const float xMax = Config::gameSize.getX();

		const bool needTurn = _direction.getY() == 0.f && (
			(_direction.getX() < 0.f && x < 0.f) ||
			(_direction.getX() > 0.f && x > xMax));
		
		if (needTurn) {
			_direction.X() = -_direction.getX();
			_mirrorX = !_mirrorX;
		}

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
}