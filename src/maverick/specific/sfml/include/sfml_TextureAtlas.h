#pragma once
#include "Rect.h"

#include <string>
#include <map>

class TextureAtlas {
public:
	bool load(const std::string& fileName, const std::string& texName);
	const Rect& getRect(const std::string &name) const;
	const std::string& getTextureName() const;

private:
	std::string _textureName;
	std::map<std::string, Rect> _rects;
};
