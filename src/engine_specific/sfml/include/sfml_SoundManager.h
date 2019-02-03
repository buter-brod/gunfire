#pragma once
#include "SoundManager.h"
#include <unordered_map>

class Sound;
typedef std::shared_ptr<Sound> SoundPtr;

class sfml_SoundManager : public SoundManager {

public:
	virtual bool LoadSound(const std::string& sName) override;
	virtual bool PlaySound(const std::string& sName, const bool loop = false) override;

	friend class SoundManager;

	~sfml_SoundManager();

protected:
	const SoundPtr getSound(const std::string& sName, const bool onlyTry = false);

private:
	sfml_SoundManager();
	std::unordered_map<std::string, SoundPtr> _sounds;
};