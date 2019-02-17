#include "sfml_Sound.h"
#include "Log.h"

Sound::Sound() noexcept {}
Sound::~Sound() {}

const std::string& Sound::getName() const {
	return _name;
}

void Sound::SetLoop(const bool loop) {
	Log::Inst()->PutErr("Error, Sound::SetLoop not implemented");
}

bool Sound::load(const std::string& name) {
	_name = name;

	return true;
}

void Sound::Play() {
	Log::Inst()->PutErr("Error, Sound::Play not implemented");
}

void Sound::Stop() {
	Log::Inst()->PutErr("Error, Sound::Stop not implemented");
}

sfml_Sound::~sfml_Sound() {
	Log::Inst()->PutMessage("~sfml_Sound: " + getName());
}

void sfml_Sound::SetLoop(const bool loop) {
	_sound.setLoop(loop);
}

bool sfml_Sound::load(const std::string& name) {

	Sound::load(name);

	const bool loadOk = _buffer.loadFromFile(name);
	if (!loadOk) {
		return false;
	}

	_sound.setBuffer(_buffer);
	return true;
}

void sfml_Sound::Play() {
	_sound.play();
}

void sfml_Sound::Stop() {
	_sound.stop();
}