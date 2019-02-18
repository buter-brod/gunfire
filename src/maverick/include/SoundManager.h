#pragma once
#include <string>
#include <memory>

class SoundManager;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;

class SoundManager {
public:

	virtual ~SoundManager() = default;

	static SoundManagerWPtr getSoundMgr();
	static void setSoundMgr(const SoundManagerWPtr& mgrPtr);

	static bool IsSoundLoaded(const std::string& snd);
	static bool LoadSound(const std::string& snd);
	static bool PlaySound(const std::string& snd, const bool loop = false);
	static bool StopSound(const std::string& snd);

protected:
	virtual bool loadSound(const std::string& sName);
	virtual bool isSoundLoaded(const std::string& sName);
	virtual bool playSound(const std::string& sName, const bool loop = false);
	virtual bool stopSound(const std::string& sName);

protected:
	SoundManager() = default;
};
