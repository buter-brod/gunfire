#include "sfml_SoundManager.h"
#include "sfml_Sound.h"

#include "Config.h"
#include "Log.h"

std::shared_ptr<SoundManager>& SoundManager::instancePtr() {
	static std::shared_ptr<SoundManager> instancePtr{ nullptr };
	return instancePtr;
}

sfml_SoundManager::~sfml_SoundManager() {
	Log::Inst()->PutMessage("~sfml_SoundManager");
}

sfml_SoundManager::sfml_SoundManager(){
	Log::Inst()->PutMessage("sfml_SoundManager");
}

SoundManager* SoundManager::Inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new sfml_SoundManager());
	}

	return ptr.get();
}

bool sfml_SoundManager::LoadSound(const std::string& sName) {

	const auto& sndIt = _sounds.find(sName);

	if (sndIt != _sounds.end()) {
		const SoundPtr foundSndPtr = sndIt->second;
		Log::Inst()->PutMessage("ResourceManager::AddSound: sound already added " + sName);
		return false;
	}

	SoundPtr sndPtr = std::make_shared<sfml_Sound>();

	const bool loadOk = sndPtr->load(CfgStatic::getResourceDir() + CfgStatic::getSoundDir() + sName);
	if (!loadOk) {
		Log::Inst()->PutErr("ResourceManager::AddSound error: unable to load " + sName);
		return false;
	}

	_sounds[sName] = sndPtr;
	Log::Inst()->PutMessage("ResourceManager::AddSound: sound added successfully: " + sName);

	return true;
}

const SoundPtr sfml_SoundManager::getSound(const std::string& sName, const bool onlyTry) {

	const auto& sndIt = _sounds.find(sName);

	if (sndIt == _sounds.end()) {

		if (!onlyTry) {
			Log::Inst()->PutErr("ResourceManager::GetSound error, not found sound " + sName);
		}
		return SoundPtr();
	}

	return sndIt->second;
}

bool sfml_SoundManager::PlaySound(const std::string& sName, const bool loop) {

	const SoundPtr snd = getSound(sName);
	if (snd) {
		snd->SetLoop(loop);
		snd->Play();
		return true;
	}

	return false;
}