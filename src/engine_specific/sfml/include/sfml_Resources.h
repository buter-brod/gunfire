#include "sfml_Sprite.h"

#include "GameObject.h"

#include <set>
#include <unordered_map>

class GraphicsPool {

public:

	SpritePtr AddSprite(const GameObjectPtr objPtr);
	SpritePtr GetSprite(const GameObjectPtr objPtr) const;
	SpritePtr GetSprite(const IDType id) const;
	bool RemoveSprite(const IDType id);

	std::set<IDType> getAvailableIDs() const;

private:
	std::unordered_map<IDType, SpritePtr> _sprites;
};