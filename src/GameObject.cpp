#include "GameObject.h"
#include "Log.h"

GameObject::GameObject(const IDType id, const std::string& name) :_id(id), _name(name) {
}

void GameObject::SetPosition(const Point& pt) {
	_position = pt;
}

void GameObject::SetSize(const Size& sz) {
	_size = sz;
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

AnimationPtr GameObject::addAnimation(const std::string& name, const unsigned int framesCount, const unsigned int fps) {

	AnimationPtr animPtr;
	const auto& animIt = _animations.find(name);

	if (animIt != _animations.end()) {

		Log::Inst()->PutMessage("GameObject::addAnimation " + name + " already added, no action needed");
		animPtr = animIt->second;
	}
	else {
		animPtr = std::make_shared<Animation>(name, framesCount, fps);
		_animations[_name] = animPtr;
	}
	
	return animPtr;
}


bool GameObject::playAnimation(const std::string& animName) {

	const auto& animIt = _animations.find(animName);

	if (animIt == _animations.end()) {
		Log::Inst()->PutErr("GameObject::playAnimation error, not found " + animName);
		return false;
	}

	_animationState._ptr = animIt->second;
	_animationState._startTime = clock();

	return true;
}

void GameObject::updateScale() {

	if (_spritePtr == nullptr) {
		Log::Inst()->PutErr("GameObject::updateScale error, _spritePtr null, obj id " + std::to_string(_id));
		return;
	}

	const auto& tRect = _spritePtr->getSpr()->getTextureRect();

	if (_size.len() == 0.f) {
		_size = { float(tRect.width), float(tRect.height) }; // will use texture size
	}

	_spritePtr->getSpr()->setScale(
		_size.getX() / tRect.width * _scale,
		_size.getY() / tRect.height * _scale);

	const Size halfSize(float(tRect.width) / 2.f, float(tRect.height) / 2.f);
	_spritePtr->getSpr()->setOrigin(halfSize.getX(), halfSize.getY());
}

void GameObject::updateAnimations() {
		
	if (_animationState.isEmpty()) {

		if (_animations.empty()) {

			addAnimation(_name, 1, 0);
		}

		playAnimation(_name);
	}

	const auto& currAnim = _animationState._ptr.lock();

	if (currAnim == nullptr) {
		Log::Inst()->PutErr("GameObject::update error, animation broken :(");
		return;
	}

	const clock_t time = clock();
	const float dtFromAnimBegin = Utils::dt(time, _animationState._startTime);
	const auto& textureRect = currAnim->GetTexRectFor(dtFromAnimBegin);

	const auto& texturePtr = textureRect.texturePtr.lock();

	if (texturePtr == nullptr) {
		Log::Inst()->PutErr("GameObject::update error, animation texture not found? " + currAnim->getName());
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

void GameObject::update(float dt) {

	if (_direction.len() == 0) {
		Log::Inst()->PutErr("GameObject::update error, invalid direction " + std::to_string(getId()));
	}

	_position += _direction.normalized() * _speed * dt;
	_rotation += _angleSpeed * dt;
	_speed += _acceleration * dt;

	updateAnimations();
	updateScale();

	_spritePtr->getSpr()->setRotation(_rotation);
	_spritePtr->getSpr()->setPosition(_position.getX(), _position.getY());
}