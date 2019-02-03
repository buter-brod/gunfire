#include "sfml_EngineComponent.h"

#include "sfml_ResourceManager.h"
#include "sfml_Particles.h"
#include "sfml_Sprite.h"

#include "GameObject.h"
#include "Log.h"
#include "Config.h"

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
	Size size((float)tSz.x, (float)tSz.y);
	return size;
}

bool sfml_EngineComponent::updateParticles(const float dt) {

	GameObjectPtr obj = getGameObject().lock();
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

	return true;
}

void sfml_EngineComponent::StopEmitters() {

	if (_particlesPtr) {
		_particlesPtr->StopEmitters();
	}
}

bool sfml_EngineComponent::updateSprite() {

	GameObjectPtr obj = getGameObject().lock();
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
		const TextureRect& textureRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
		const auto& texturePtr = textureRect.texturePtr.lock();

		if (!_spritePtr) {
			_spritePtr = std::make_shared<Sprite>(*texturePtr, textureRect.rect);
		}
		else {
			_spritePtr->getSpr()->setTexture(*texturePtr->getTex());
			_spritePtr->getSpr()->setTextureRect(Utils::toSfmlRect(textureRect.rect));
		}
	}

	if (_spritePtr) {

		const TextureRect& textureRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
		const auto& texturePtr = textureRect.texturePtr.lock();

		const float scale = obj->GetScale();
		const Size& size = obj->GetSize();
		const bool mirrorX = obj->GetMirrorX();

		const unsigned int tW = (unsigned int)textureRect.rect._size.getX();
		const unsigned int tH = (unsigned int)textureRect.rect._size.getY();

		const Size wndScale = CfgStatic::windowSize / CfgStatic::gameSize;

		Size sprScale = size / textureRect.rect._size * scale * wndScale;
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
		auto shaderPtr = sfml_ResourceManager::Inst()->GetShader(shaderName);

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

	auto objPtr = getGameObject().lock();

	if (!objPtr) {
		Log::Inst()->PutErr("EngineComponent::Update error, no valid object associated");
		return;
	}
	
	updateSprite();
	updateShader();
	updateParticles(dt);
}

