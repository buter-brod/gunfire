#pragma once
#include "EngineComponent.h"
#include "MiscForward.h"
#include "sfml_Texture.h"

class Sprite;
class Particles;

typedef std::shared_ptr<Sprite> SpritePtr;
typedef std::shared_ptr<Particles> ParticlesPtr;

class sfml_EngineComponent : public EngineComponent {
public:

	explicit sfml_EngineComponent(GameObjectWPtr objPtr) : EngineComponent(objPtr) {}

	void Update(const float dt) override;
	virtual Rect GetSpriteRect() const override;
	virtual Size GetTextureSize() const override;
	
	SpritePtr GetSprite() const { return _spritePtr; }

#ifndef NO_THOR
	ParticlesPtr GetParticles() const { return _particlesPtr; }
#endif

	virtual void StopEmitters() override;

protected:
	bool updateSprite();
	bool updateShader();
	bool updateParticles(const float dt);

private:
	SpritePtr _spritePtr;
	TextureRect _latestTexRect;

	std::string _textureName;
	std::string _shaderName;
	std::string _particlesName;

#ifndef NO_THOR
	ParticlesPtr _particlesPtr;
#endif
};