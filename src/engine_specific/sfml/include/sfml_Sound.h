#pragma once
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

class Sound {

public:
	Sound() noexcept;
	virtual ~Sound();

	virtual bool load(const std::string& name);

	virtual void Play();
	virtual void SetLoop(const bool loop = true);

protected:
	const std::string& getName() const;

private:
	std::string _name;
};

typedef std::shared_ptr<Sound> SoundPtr;


class sfml_Sound : public Sound {

public:
	sfml_Sound() noexcept;
	~sfml_Sound();

	bool load(const std::string& name) override;

	void Play() override;
	void SetLoop(const bool loop = true) override;

private:
	sf::Sound _sound;
	sf::SoundBuffer _buffer;
};

