#include "sfml_Resources.h"
#include "sfml_Texture.h"
#include "sfml_ResourceManager.h"

#include "Log.h"

SpritePtr GraphicsPool::AddSprite(const GameObjectPtr objPtr) {

	const std::string& sprName = objPtr->GetCurrentSpriteName();
	if (sprName.empty()) {
		return SpritePtr();
	}

	TextureRect textureRect = sfml_ResourceManager::Inst()->GetTexture(sprName);
	const auto& texturePtr = textureRect.texturePtr.lock();
	SpritePtr spritePtr = std::make_shared<Sprite>(*texturePtr, textureRect.rect);

	const IDType id = objPtr->getId();

	if (_sprites.count(id) > 0) {
		Log::Inst()->PutErr("GraphicsPool::AddSprite error, already has sprite for obj " + objPtr->getFullName() + ", will replace! ");
	}

	_sprites[id] = spritePtr;
	return spritePtr;
}

SpritePtr GraphicsPool::GetSprite(const IDType id) const {
	
	SpritePtr spritePtr = nullptr;
	const auto& sprIt = _sprites.find(id);

	if (sprIt != _sprites.end()) {
		spritePtr = sprIt->second;
	}

	return spritePtr;
}

SpritePtr GraphicsPool::GetSprite(const GameObjectPtr objPtr) const {
	return GetSprite(objPtr->getId());
}

bool GraphicsPool::RemoveSprite(const IDType id) {
	
	const auto& sprIt = _sprites.find(id);
	if (sprIt != _sprites.end()) {
		_sprites.erase(id);
		return true;
	}
	else {
		Log::Inst()->PutErr("GraphicsPool::RemoveSprite error, obj " + std::to_string(id) + ", has no sprite.");
	}

	return false;
}

std::set<IDType> GraphicsPool::getAvailableIDs() const {

	std::set<IDType> availables;
	for (const auto& sprPair : _sprites) {
		availables.insert(sprPair.first);
	}

	return availables;
}