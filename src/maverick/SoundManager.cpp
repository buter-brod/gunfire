#include "SoundManager.h"
#include "Log.h"

SoundManagerWPtr& mgrPtr() {
	static SoundManagerWPtr mgrPtr;
	return mgrPtr;
}

void SoundManager::setSoundMgr(const SoundManagerWPtr& mp) { mgrPtr() = mp; }
SoundManagerWPtr SoundManager::getSoundMgr() { return mgrPtr(); }

bool SoundManager::loadSound(const std::string& sName) {
	return false;
};

bool SoundManager::isSoundLoaded(const std::string& sName) {
	return false;
}

bool SoundManager::stopSound(const std::string& sName) {
	return false;
};

bool SoundManager::playSound(const std::string& sName, const bool loop) {
	return false;
};

bool SoundManager::StopSound(const std::string& snd) {

	Log::Inst()->PutMessage("SoundManager::StopSound " + snd);

	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::PlaySound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->stopSound(snd);
}

bool SoundManager::PlaySound(const std::string& snd, const bool loop) {

	Log::Inst()->PutMessage("SoundManager::PlaySound " + snd);

	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::PlaySound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->playSound(snd, loop);
}

bool SoundManager::IsSoundLoaded(const std::string& snd) {
	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::LoadSound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->isSoundLoaded(snd);
}

bool SoundManager::LoadSound(const std::string& snd) {
	auto sndMgr = SoundManager::getSoundMgr().lock();
	if (!sndMgr) {
		Log::Inst()->PutErr("SoundManager::LoadSound error, sound mgr not set correctly");
		return false;
	}

	return sndMgr->loadSound(snd);
}