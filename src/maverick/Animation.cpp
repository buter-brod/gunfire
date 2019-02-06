#include "Animation.h"
#include "Log.h"

Animation::Animation(const std::string& name, unsigned int size, unsigned int fps) : _name(name), _size(size), _fps(fps) {

	if (size == 0) {
		Log::Inst()->PutErr("Animation::Animation error, size cannot be 0... " + name);
	}

	if (size == 1 && fps > 0) {
		Log::Inst()->PutErr("Animation::Animation warning: if size == 1, fps should be 0... " + name);
		_fps = 0;
	}
}

std::string Animation::GetFrameName(float dt) const {
	
	std::string frameName = _name;
	unsigned int frameInd = getFrameIndex(dt);

	if (_size > 1) {
		const auto dotPos = frameName.find("."); // if names contain file extension, put frame number at the end of the name-part.
		frameName.insert(dotPos, std::to_string(frameInd));
	}

	return frameName;
}

unsigned int Animation::getFrameIndex(float dt) const {

	unsigned int frameInd = 0;

	if (_size > 1) {
		frameInd = unsigned(dt * _fps) % _size;
	}
	else if (_size == 0) {
		Log::Inst()->PutErr("Animation::getFrameIndex error, _size empty for " + _name);
	}

	return frameInd;
}

