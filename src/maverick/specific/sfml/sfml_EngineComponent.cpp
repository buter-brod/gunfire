#include "sfml_EngineComponent.h"

#include "sfml_ResourceManager.h"
#include "sfml_Particles.h"
#include "sfml_Sprite.h"

#include "GameObject.h"
#include "Log.h"
#include "CFCfgStatic.h"

Rect sfml_EngineComponent::GetSpriteRect() const {

	if (!_spritePtr) {
		return EngineComponent::GetSpriteRect();
	}

	const auto& sfmlRect = _spritePtr->getSpr()->getTextureRect();
	const auto& rect = Utils::fromSfmlRect(sfmlRect);
	return rect;
}

Size sfml_EngineComponent::GetTextureSize() const {

	if (!_spritePtr) {
		return EngineComponent::GetTextureSize();
	}

	const auto& tSz = _spritePtr->getSpr()->getTexture()->getSize();
	const Size size((float)tSz.x, (float)tSz.y);
	return size;
}

bool sfml_EngineComponent::updateParticles(const float dt) {
#ifndef NO_THOR

	const GameObjectPtr obj = getGameObject().lock();
	const std::string& currParticlesName = obj->getParticlesName();
	const bool changed = (_particlesName != currParticlesName);
	_particlesName = currParticlesName;

	if (changed) {
		if (currParticlesName.empty() && _particlesPtr) {
			_particlesPtr = nullptr;
			return true;
		}

		_particlesPtr = Particles::Build(currParticlesName);
	}

	if (!_particlesPtr) {
		return false;
	}
		
	const Point& emitterPos = obj->GetEmitterPosition();
	_particlesPtr->Update(dt, emitterPos);

#endif

	return true;
}

void sfml_EngineComponent::StopEmitters() {
#ifndef NO_THOR
	if (_particlesPtr) {
		_particlesPtr->StopEmitters();
	}
#endif
}

bool sfml_EngineComponent::updateSprite() {

	const GameObjectPtr obj = getGameObject().lock();
	const std::string& sprName = obj->GetCurrentSpriteName();
	const bool changed = (_textureName != sprName);
	_textureName = sprName;

	if (sprName.empty()) {
		if (_spritePtr) {
			_spritePtr = nullptr;
			return true;
		}
		return false;
	}

	if (changed) {
		_latestTexRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
		const auto& texturePtr = _latestTexRect.texturePtr.lock();

		if (!_spritePtr) {
			_spritePtr = std::make_shared<Sprite>(*texturePtr, _latestTexRect.rect);
		}
		else {
			_spritePtr->getSpr()->setTexture(*texturePtr->getTex());
			_spritePtr->getSpr()->setTextureRect(Utils::toSfmlRect(_latestTexRect.rect));
		}
	}

	if (_spritePtr) {

		const float scale = obj->GetScale();
		const Size& size = obj->GetSize();
		const bool mirrorX = obj->GetMirrorX();

		const auto tW = (unsigned int)_latestTexRect.rect._size.getX();
		const auto tH = (unsigned int)_latestTexRect.rect._size.getY();

		const Size wndScale = CfgStatic::windowSize / CfgStatic::gameSize;

		Size sprScale = size / _latestTexRect.rect._size * scale * wndScale;
		sprScale.X() *= (mirrorX ? -1.f : 1.f);

		_spritePtr->getSpr()->setScale(Utils::toSfmlVector2f(sprScale));

		const auto rot = obj->GetRotation();
		const auto pos = obj->GetPosition() * wndScale;

		_spritePtr->getSpr()->setRotation(rot);
		_spritePtr->getSpr()->setPosition(Utils::toSfmlVector2f(pos));

		const Size halfSize(float(tW) / 2.f, float(tH) / 2.f);
		_spritePtr->getSpr()->setOrigin(halfSize.getX(), halfSize.getY());
	}

	return true;
}

bool sfml_EngineComponent::updateShader() {

	const std::string& shaderName = getGameObject().lock()->getShaderName();
	const bool changed = (shaderName != _shaderName);

	if (!changed) {
		return false;
	}

	if (!shaderName.empty()) {
		const auto shaderPtr = sfml_ResourceManager::Inst()->GetShader(shaderName);

		if (!shaderPtr) {
			Log::Inst()->PutErr("sfml_EngineComponent::updateShader error: unable to get " + shaderName);
			return false;
		}
		else {
			setShader(shaderPtr);
			_shaderName = shaderName;
		}
	}

	return true;
}

void sfml_EngineComponent::Update(const float dt) {

	const auto objPtr = getGameObject().lock();

	if (!objPtr) {
		Log::Inst()->PutErr("EngineComponent::Update error, no valid object associated");
		return;
	}
	
	updateSprite();
	updateShader();
	updateParticles(dt);
}

