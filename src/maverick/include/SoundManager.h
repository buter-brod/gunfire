#pragma once
#include <string>
#include <memory>

class SoundManager;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;

class SoundManager {
public:

	virtual ~SoundManager() = default;

	static SoundManagerWPtr getSoundMgr();
	static void setSoundMgr(SoundManagerWPtr mgrPtr);
	
	static bool LoadSound(const std::string& sName);
	static bool PlaySound(const std::string& sName, const bool loop = false);
	static bool StopSound(const std::string& sName);

protected:
	virtual bool loadSound(const std::string& sName);
	virtual bool playSound(const std::string& sName, const bool loop = false);
	virtual bool stopSound(const std::string& sName);

protected:
	SoundManager() = default;
};
