#include "SoundManager.h"

void SoundManager::ResetInst() {

	auto& ptr = instancePtr();
	if (ptr != nullptr) {
		ptr.reset();
	}
}

SoundManager::SoundManager() {	
}

SoundManager::~SoundManager() {
}