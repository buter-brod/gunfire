#include "Sound.h"

Sound::Sound() noexcept {}

Sound::~Sound() {
	Log::Inst()->PutMessage("~Sound: " + _name);
}

void Sound::SetLoop(const bool loop) {
	_sound.setLoop(loop);
}

bool Sound::load(const std::string& name) {

	_name = name;

	const bool loadOk = _buffer.loadFromFile(name);
	if (!loadOk) {
		return false;
	}

	_sound.setBuffer(_buffer);
	return true;
}

void Sound::Play() {
	_sound.play();
}