#pragma once
#include "Point.h"

#include <unordered_map>
#include <memory>

class Shader;
class Animation;
class EngineComponent;

typedef std::shared_ptr<EngineComponent> EngineComponentPtr;
typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::shared_ptr<Animation> AnimationPtr;

static const float basedOnAnimation{ -1.f };

typedef unsigned long IDType;

class Game;
typedef std::weak_ptr<Game> GameWPtr;

class GameObject {

public:
	GameObject(const IDType id, const std::string& name, const std::string& idleAnim);

	virtual ~GameObject();

	AnimationPtr AddAnimation(const std::string& name);
	AnimationPtr AddAnimation(const std::string& name, const unsigned int framesCount, const unsigned int fps);
	
	virtual void Update(const float dt, const float gameTime);

	void SetScale(const float scale);
	void SetSize(const Size& sz);
	void SetPosition(const Point& pt);
	void SetDirection(const Point& d);
	void SetSpeed(const float s);
	void SetAngleSpeed(const float s);
	void SetMirrorX(const bool mirrorX);

	/* call this to check whether object can be removed from scene immediately. 
	Returns TRUE if it's ok to remove it now,
	returns FALSE if it wants to do something first. (in this case correct removal time will be managed via "deadState")
	*/
	virtual bool RequestKill(const std::string& reason);

	std::string GetCurrentSpriteName() const;
	virtual Point GetEmitterPosition() const;

	struct State;
	typedef std::shared_ptr<State> StatePtr;

	void ChangeState(StatePtr newState);
	const std::string GetState() const;
	const std::string GetAnimation() const;
	
	float GameObject::GetEffectCoeff(const float maxDuration, const float minVal, const float maxVal) const;

	std::string getFullName() const;

	StatePtr getIdleState() { return _idleState; }

	const Point& GetPosition()  const { return _position; }
	const Point& GetDirection() const { return _direction; }
	const Size& GetSize()       const { return _size; }
	
	IDType getId() const { return _id; }

	bool  GetMirrorX()    const { return _mirrorX; }
	float GetSpeed()      const { return _speed; }
	float GetScale()      const { return _scale; }
	float GetRotation()   const { return _rotation; }

	std::string getShaderName() const;
	std::string getParticlesName() const;

	EngineComponentPtr GetEngineComponent() const;
	void SetEngineComponent(EngineComponentPtr component);

	virtual ShaderPtr GetShader();

	struct State {

		explicit State(const std::string& name) : _name(name) {}

		std::string _name;
		std::string _animation;
		std::string _sound;
		std::string _soundEnd;
		std::string _shader;
		std::string _particles;

		float _duration{ 0.f };
		float _startTime{ 0 };

		StatePtr _nextState;
		bool isEmpty() const { return _name.empty(); }

		static StatePtr New(const std::string& name) {
			return std::make_shared<State>(name);
		}
	};

protected:
	inline StatePtr getState() const { return _state; };
	inline float getGameSimulationTime() const { return _gameSimulationTime; }
	void updateState();

	AnimationPtr getAnimation(const std::string& animName, const bool onlyTry = false) const;
	virtual void onStateUpdate(const StatePtr prevState);

private:
	float _gameSimulationTime{ 0.f };

	StatePtr _state;
	StatePtr _idleState;

	EngineComponentPtr _engineComponent{ nullptr };

	std::unordered_map<std::string, AnimationPtr> _animations;

	Point _position;
	Point _direction{0.f, 1.f};
	Size _size;

	float _scale       { 1.f };
	float _rotation    { 0.f }; // degrees!
	float _speed       { 0.f };
	float _angleSpeed  { 0.f }; // degrees!
	float _acceleration{ 0.f };

	bool _mirrorX{ false };

	IDType _id{ 0 };

	std::string _name;
	std::string _idleAnimation;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;