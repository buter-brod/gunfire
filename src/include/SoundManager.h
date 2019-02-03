#pragma once
#include <string>
#include <memory>

class SoundManager {
public:

	SoundManager();
	virtual ~SoundManager();

	static SoundManager* Inst();
	static void ResetInst();

	virtual bool LoadSound(const std::string& sName) = 0;
	virtual bool PlaySound(const std::string& sName, const bool loop = false) = 0;

private:
	static std::shared_ptr<SoundManager>& instancePtr();
};