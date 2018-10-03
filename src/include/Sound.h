#pragma once
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <memory>
#include <string>
#include "Log.h"

class Sound {

public: 
	Sound() noexcept;
	~Sound();

	bool load(const std::string& name);
	
	void Play();
	void SetLoop(const bool loop = true);

private:

	sf::Sound _sound;
	sf::SoundBuffer _buffer;
	std::string _name;
};

typedef std::shared_ptr<Sound> SoundPtr;
