#pragma once
#include "EngineComponent.h"
#include <memory>

class Particles;
class Sprite;

typedef std::shared_ptr<Sprite> SpritePtr;
typedef std::shared_ptr<Particles> ParticlesPtr;

class sfml_EngineComponent : public EngineComponent {
public:

	explicit sfml_EngineComponent(GameObjectPtr objPtr) : EngineComponent(objPtr) {}

	void Update(const float dt) override;
	virtual Rect GetSpriteRect() const override;
	virtual Size GetTextureSize() const override;
	
	SpritePtr GetSprite() const { return _spritePtr; }
	ParticlesPtr GetParticles() const { return _particlesPtr; }

	virtual void StopEmitters() override;

protected:
	bool updateSprite();
	bool updateShader();
	bool updateParticles(const float dt);

private:
	SpritePtr _spritePtr;
	ParticlesPtr _particlesPtr;

	std::string _textureName;
	std::string _shaderName;
	std::string _particlesName;
};