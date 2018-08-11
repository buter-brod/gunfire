#pragma once
#include "Animation.h"
#include <unordered_map>
#include "Utils.h"

class GameObject {

public:
	GameObject(const IDType id, const std::string& name);

	virtual ~GameObject();

	AnimationPtr AddAnimation(const std::string& name, const unsigned int framesCount, const unsigned int fps);
	bool PlayAnimation(const std::string& animName);
	void Update(float dt);

	void SetScale(const float scale);
	void SetSize(const Size& sz);
	void SetPosition(const Point& pt);
	void SetDirection(const Point& d);
	void SetSpeed(const float s);
	void SetAngleSpeed(const float s);
	void SetMirrorX(const bool mirrorX);

	const Point& GetPosition() const { return _position; }
	const Point& GetDirection() const { return _direction; }
	const Size& GetSize() const { return _size; }

	IDType getId() const { return _id; }
	SpritePtr getSprite() { return _spritePtr; }

protected:

	void updateScale();
	void updateAnimations();

	struct AnimationState {

		AnimationWPtr _ptr;
		time_us _startTime{ 0 };
		bool isEmpty() const { return _startTime == 0; }
	};

	std::unordered_map<std::string, AnimationPtr> _animations;
	AnimationState _animationState;

	Point _position;
	Point _direction{1.f, 0.f};
	Size _size;
	float _scale{ 1.f };
	
	float _rotation{ 0.f };
	float _speed{ 0.f };
	float _angleSpeed{ 0.f };
	float _acceleration{ 0.f };

	bool _mirrorX{ false };

	IDType _id{ 0 };

	std::string _name;
	SpritePtr _spritePtr;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;