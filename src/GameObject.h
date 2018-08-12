#pragma once
#include "Animation.h"
#include <unordered_map>
#include "Utils.h"

static const float basedOnAnimation{ -1.f };

class GameObject {

public:
	GameObject(const IDType id, const std::string& name, const std::string& idleAnim);

	virtual ~GameObject();

	AnimationPtr AddAnimation(const std::string& name);
	AnimationPtr AddAnimation(const std::string& name, const unsigned int framesCount, const unsigned int fps);
	AnimationPtr GetAnimation(const std::string& animName, const bool onlyTry = false);
	
	void Update(float dt);

	void SetScale(const float scale);
	void SetSize(const Size& sz);
	void SetPosition(const Point& pt);
	void SetDirection(const Point& d);
	void SetSpeed(const float s);
	void SetAngleSpeed(const float s);
	void SetMirrorX(const bool mirrorX);

	struct State;
	typedef std::shared_ptr<State> StatePtr;

	void ChangeState(StatePtr newState);
	const std::string GetState() const;

	StatePtr getIdleState() { return _idleState; }

	const Point& GetPosition() const { return _position; }
	const Point& GetDirection() const { return _direction; }
	const Size& GetSize() const { return _size; }
	const float GetSpeed() const { return _speed; }

	IDType getId() const { return _id; }
	SpritePtr getSprite() { return _spritePtr; }

	struct State {

		State(const std::string& name) : _name(name) {}

		std::string _name;
		std::string _animation;
		std::string _sound;
		std::string _soundEnd;

		float _duration{ 0.f };
		time_us _startTime{ 0 };

		StatePtr _nextState;
		bool isEmpty() const { return _name.empty(); }
	};

protected:

	void updateScale();
	void updateState();
	void updateAnimations();

	virtual void onStateUpdate();

	StatePtr _state;
	StatePtr _idleState;
	SpritePtr _spritePtr;

	std::unordered_map<std::string, AnimationPtr> _animations;

	Point _position;
	Point _direction{0.f, 1.f};
	Size _size;

	float _scale       { 1.f };
	float _rotation    { 0.f };
	float _speed       { 0.f };
	float _angleSpeed  { 0.f };
	float _acceleration{ 0.f };

	bool _mirrorX{ false };

	IDType _id{ 0 };

	std::string _name;
	std::string _idleAnimation;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;