#pragma once
#include "Animation.h"
#include <time.h>
#include <unordered_map>
#include "Utils.h"

class GameObject {

public:
	GameObject(const IDType id, const std::string& name);
	bool playAnimation(const std::string& animName);
	void update(float dt);

	void updateScale();
	void updateAnimations();

	void SetSize(const Size& sz);
	void SetPosition(const Point& pt);

	IDType getId() const { return _id; }

	SpritePtr getSprite() { return _spritePtr; }

protected:

	struct AnimationState {
		AnimationWPtr _ptr;
		clock_t _startTime{ 0 };

		bool isEmpty() const { return _startTime == 0; }
	};

	std::unordered_map<std::string, AnimationPtr> _animations;

	AnimationState _animationState;

	Point _position;
	Point _direction{1.f, 0.f};
	Size _size;
	
	float _rotation{ 0.f };
	float _speed{ 0.f };
	float _angleSpeed{ 0.f };
	float _acceleration{ 0.f };

	IDType _id{ 0 };

	std::string _name;

	SpritePtr _spritePtr;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;