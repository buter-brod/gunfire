#include "GameObject.h"
#include "Log.h"

GameObject::GameObject(const IDType id, const std::string& name) :_id(id), _name(name) {
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

void GameObject::SetPosition(const Point& pt) {
	_position = pt;
}

void GameObject::SetSize(const Size& sz) {
	_size = sz;
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
		_size.getX() / tRect.width,
		_size.getY() / tRect.height);
}

void GameObject::updateAnimations() {
		
	if (_animationState.isEmpty()) {

		if (_animations.empty()) {
			AnimationPtr defaultAnimationPtr = std::make_shared<Animation>(_name, 2);
			_animations[_name] = defaultAnimationPtr;
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

	_spritePtr->getSpr()->setPosition(_position.getX(), _position.getY());
}