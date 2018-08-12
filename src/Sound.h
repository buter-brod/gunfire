#pragma once
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <string>

// PATH=%PATH%;$(ProjectDir)..\Resources

class Sound {

public: 
	Sound() {}

	bool load(const std::string& name) {

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
};

typedef std::shared_ptr<Sound> SoundPtr;
