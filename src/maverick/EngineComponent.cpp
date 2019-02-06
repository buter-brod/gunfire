#include "EngineComponent.h"
#include "Log.h"

GameObjectWPtr EngineComponent::getGameObject() const {

	const auto objPtr = _gameObjectWPtr.lock();

	if (objPtr == nullptr) {
		Log::Inst()->PutErr("EngineComponent::getGameObject error, no valid object associated");
	}

	return objPtr;
}

void EngineComponent::Update(const float dt) {}

Size EngineComponent::GetSpriteSize() const {

	const auto& sprRect = GetSpriteRect();
	return { sprRect._size.getX(), sprRect._size.getY() }; 
}

Rect EngineComponent::GetSpriteRect() const {
	Log::Inst()->PutErr("EngineComponent::GetSpriteRect error, not implemented");
	return Rect();
}

Size EngineComponent::GetTextureSize() const {
	Log::Inst()->PutErr("EngineComponent::GetTextureSize error, not implemented");
	return Size();
}

void EngineComponent::StopEmitters() {
	Log::Inst()->PutErr("EngineComponent::StopEmitters error, not implemented");
}
