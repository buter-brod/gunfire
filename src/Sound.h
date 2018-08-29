#pragma once
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <string>
#include "Log.h"

class Sound {

public: 
	Sound() noexcept {}
	~Sound() {
		Log::Inst()->PutMessage("~Sound: " + _name);
	}

	bool load(const std::string& name) {

		_name = name;

		const bool loadOk = _buffer.loadFromFile(name);
		if (!loadOk) {
			return false;
		}

		_sound.setBuffer(_buffer);
		return true;
	}
	
	sf::Sound& get() {
		return _sound;
	}

private:
	sf::Sound _sound;
	sf::SoundBuffer _buffer;
	std::string _name;
};

typedef std::shared_ptr<Sound> SoundPtr;
