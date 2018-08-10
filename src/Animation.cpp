#include "Animation.h"
#include "Log.h"
#include "TextureManager.h"

Animation::Animation(const std::string& name, unsigned int size, unsigned int fps) {
	load(name, size, fps);
}

bool Animation::load(const std::string& name, unsigned int size, unsigned int FPS) {

	if (size < 1) {
		Log::Inst()->PutErr("Animation::load " + name + ", invalid size");
		return false;
	}

	for (unsigned int ind = 0; ind < size; ind++) {
		
		const std::string frameName = name + std::to_string(ind) + ".png";
		const auto& texRect = TextureManager::Inst()->GetTexture(frameName);
		const auto& texPtr = texRect.texturePtr.lock();

		if (texPtr == nullptr) {
			Log::Inst()->PutErr("Animation::load error, texture not found for frame " + frameName);
			return false;
		}

		const Texture& tex = *texPtr;
		const auto& rect = texRect.rect;

		_frames.emplace_back(tex, rect);
	}

	return true;
}