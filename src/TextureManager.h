#pragma once

#include <unordered_map>
#include <string>
#include <memory>

namespace sf {
	class Texture;
}

typedef sf::Texture Texture;
typedef std::shared_ptr<Texture> TexturePtr;

class TextureManager {
public:

	TextureManager::TextureManager();
	virtual ~TextureManager();

	const TexturePtr addTexture(const std::string& tName);
	bool removeTexture(const std::string& tName);
	const TexturePtr getTexture(const std::string& tName);

protected:
private:
	std::unordered_map<std::string, TexturePtr> _textures;
};