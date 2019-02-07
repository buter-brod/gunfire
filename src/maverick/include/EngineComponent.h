#pragma once
#include "Rect.h"

#include <memory>

class GameObject;
class Shader;

typedef std::shared_ptr<Shader> ShaderPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;
typedef std::shared_ptr<GameObject> GameObjectPtr;

class EngineComponent {

public:
	explicit EngineComponent(GameObjectPtr objPtr) : _gameObjectWPtr(objPtr) {}
	virtual ~EngineComponent() {}

	virtual void Update(const float dt);

	virtual Size GetSpriteSize() const;
	virtual Rect GetSpriteRect() const;
	virtual Size GetTextureSize() const;

	virtual void StopEmitters();

	ShaderPtr GetShader() const { return _shaderPtr; }

protected:
	GameObjectWPtr getGameObject() const;
	void setShader(ShaderPtr shader) { _shaderPtr = shader; }

private:
	GameObjectWPtr _gameObjectWPtr;
	ShaderPtr _shaderPtr;
};

typedef std::shared_ptr<EngineComponent> EngineComponentPtr;



