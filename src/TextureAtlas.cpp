#include "TextureAtlas.h"
#include <fstream>
#include "Config.h"
#include "Log.h"

/* simpified reader of mtpf format, google it: Moony-TexturePacker */

const std::string& TextureAtlas::getTextureName() const {
	return _textureName;
}

bool TextureAtlas::load(const std::string& descFileName, const std::string& texName) {

	_textureName = texName;

	std::ifstream input_file(CfgStatic::getResourceDir() + descFileName, std::ifstream::in | std::ifstream::binary);
	if (!input_file) {

		Log::Inst()->PutErr("TextureAtlas::load ERROR: " + descFileName);
		return false;
	}

	std::string input;

	while (input_file >> input) {
		size_t nameLen;
		(input_file >> nameLen).ignore(1);
		input.resize(nameLen);
		input_file.read(&input[0], nameLen);

		size_t imgCount;
		input_file >> imgCount;

		for (size_t index = 0; index < imgCount; index++){
			size_t imgNameLen;
			(input_file >> imgNameLen).ignore(1);
			input.resize(imgNameLen);
			input_file.read(&input[0], imgNameLen);

			Rect texture_rect;
			input_file >> texture_rect.left >> texture_rect.top >> texture_rect.width >> texture_rect.height;
			_rects[input] = texture_rect;
		}
	}
	input_file.close();

	Log::Inst()->PutMessage("TextureAtlas::load success: " + descFileName + " loaded " + std::to_string(_rects.size()) + " images");

	return true;
}

const Rect& TextureAtlas::getRect(const std::string& name) const {

	static const Rect nullRect;
	const auto& rectIt = _rects.find(name);

	if (rectIt == _rects.end()) {
		return nullRect;
	}

	const Rect& rect = rectIt->second;

	return rect;
}
