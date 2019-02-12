#pragma once
#include "MiscForward.h"
#include "Rect.h"

class EngineComponent {

public:
	explicit EngineComponent(GameObjectWPtr objPtr) : _gameObjectWPtr(objPtr) {}
	virtual ~EngineComponent() = default;

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



