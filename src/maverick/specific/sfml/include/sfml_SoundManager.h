#pragma once
#include "SoundManager.h"
#include <map>

class Sound;
typedef std::shared_ptr<Sound> SoundPtr;

class sfml_SoundManager : public SoundManager {
public:

	static void Create();

	virtual bool loadSound(const std::string& sName) override;
	virtual bool playSound(const std::string& sName, const bool loop = false) override;
	virtual bool stopSound(const std::string& sName) override;

	friend class SoundManager;

	~sfml_SoundManager();

protected:
	SoundPtr getSound(const std::string& sName, const bool onlyTry = false);

private:
	sfml_SoundManager();
	std::map<std::string, SoundPtr> _sounds;
	static std::shared_ptr<sfml_SoundManager>& instancePtr();
};