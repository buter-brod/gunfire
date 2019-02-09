#include "SoundManager.h"
#include "Log.h"

SoundManagerWPtr& mgrPtr() {
	static SoundManagerWPtr mgrPtr;
	return mgrPtr;
}

void SoundManager::setSoundMgr(SoundManagerWPtr mp) { mgrPtr() = mp; }
SoundManagerWPtr SoundManager::getSoundMgr() { return mgrPtr(); }

bool SoundManager::loadSound(const std::string& sName) {
	return false;
};

bool SoundManager::playSound(const std::string& sName, const bool loop) {
	return false;
};

bool SoundManager::PlaySound(const std::string& snd, const bool loop) {
	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::PlaySound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->playSound(snd, loop);
}

bool SoundManager::LoadSound(const std::string& snd) {
	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::LoadSound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->loadSound(snd);
}