#include "Animation.h"
#include "Log.h"
#include "TextureManager.h"

Animation::Animation(const std::string& name, unsigned int size, unsigned int fps) : _name(name), _size(size), _fps(fps) {
}

TextureRect Animation::GetTexRectFor(float dt) {

	const unsigned int frameInd = unsigned(dt * _fps) % _size;
	const TextureRect& result = getTexRectFor(frameInd);
	return result;
}

TextureRect Animation::getTexRectFor(const unsigned int index) {

	TextureRect textureRect;

	if (index >= _size) {
		Log::Inst()->PutErr("Animation::GetSpriteFor error, index " + std::to_string(index) + " out of bounds ( " + _name + ")");
		return textureRect;
	}

	std::string frameName = _name;

	if (index == 0) {
		textureRect = TextureManager::Inst()->GetTexture(frameName, true);
	}

	if (textureRect.isEmpty()) {
		const auto dotPos = frameName.find(".");

		if (dotPos != std::string::npos) {
			frameName.insert(dotPos, std::to_string(index));
			textureRect = TextureManager::Inst()->GetTexture(frameName);
		}		
	}

	if (textureRect.isEmpty()) {
		Log::Inst()->PutErr("Animation::load error, texture not found for frame " + frameName);
	}

	return textureRect;
}
