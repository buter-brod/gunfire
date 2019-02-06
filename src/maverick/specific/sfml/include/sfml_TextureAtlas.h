#pragma once
#include "Rect.h"

#include <string>
#include <unordered_map>

class TextureAtlas {
public:
	bool load(const std::string& fileName, const std::string& texName);
	const Rect& getRect(const std::string &name) const;
	const std::string& getTextureName() const;

private:
	std::string _textureName;
	std::unordered_map<std::string, Rect> _rects;
};
