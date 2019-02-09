#include "Animation.h"
#include "Log.h"

Animation::Animation(const std::string& name, const unsigned int size, const unsigned int fps) : _name(name), _size(size), _fps(fps) {

	if (size == 0) {
		Log::Inst()->PutErr("Animation::Animation error, size cannot be 0... " + name);
	}

	if (size == 1 && fps > 0) {
		Log::Inst()->PutErr("Animation::Animation warning: if size == 1, fps should be 0... " + name);
		_fps = 0;
	}

	if (_size > 1) {
		for (unsigned int ind = 0; ind < _size; ind++) {
			std::string frameName = _name;
			const auto dotPos = frameName.find('.'); // if names contain file extension, put frame number at the end of the name-part.
			frameName.insert(dotPos, std::to_string(ind));
			_frames.push_back(frameName);
		}		
	}
}

const std::string& Animation::GetFrameName(const float dt) const {
	
	if (_size == 1) {
		return _name;
	}

	const unsigned int frameInd = getFrameIndex(dt);

	if (frameInd >= _frames.size()) {
		Log::Inst()->PutErr("Animation::GetFrameName error, _size less than " + std::to_string(frameInd) + ", animation " + _name);
		static std::string emptyStr;
		return emptyStr;
	}

	const std::string& frameName = _frames.at(frameInd);
	return frameName;
}

unsigned int Animation::getFrameIndex(const float dt) const {

	unsigned int frameInd = 0;

	if (_size > 1) {
		frameInd = unsigned(dt * _fps) % _size;
	}
	else if (_size == 0) {
		Log::Inst()->PutErr("Animation::getFrameIndex error, _size empty for " + _name);
	}

	return frameInd;
}

