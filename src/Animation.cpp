#include "Animation.h"
#include "Log.h"
#include "ResourceManager.h"

Animation::Animation(const std::string& name, unsigned int size, unsigned int fps) : _name(name), _size(size), _fps(fps) {

	if (size == 0) {
		Log::Inst()->PutErr("Animation::Animation error, size cannot be 0... " + name);
	}

	if (size == 1 && fps > 0) {
		Log::Inst()->PutErr("Animation::Animation warning: if size == 1, fps should be 0... " + name);
		_fps = 0;
	}
}

TextureRect Animation::GetTexRectFor(float dt) {

	unsigned int frameInd = 0;

	if (_size > 1) {
		frameInd = unsigned(dt * _fps) % _size;
	}
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
		textureRect = ResourceManager::Inst()->GetTexture(frameName, true);
	}

	if (textureRect.isEmpty()) {
		const auto dotPos = frameName.find(".");

		if (dotPos != std::string::npos) {
			frameName.insert(dotPos, std::to_string(index));
			textureRect = ResourceManager::Inst()->GetTexture(frameName);
		}		
	}

	if (textureRect.isEmpty()) {
		Log::Inst()->PutErr("Animation::load error, texture not found for frame " + frameName);
	}

	return textureRect;
}
