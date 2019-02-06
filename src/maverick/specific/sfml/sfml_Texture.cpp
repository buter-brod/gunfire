#include "sfml_Texture.h"
#include "Log.h"
#include "sfml_Utils.h"

const sf::Texture* sfml_Texture::getTex() const {
	return &_tex;
}

bool sfml_Texture::loadFromFile(const std::string& filename, const Rect& area) {

	_name = filename;
	return _tex.loadFromFile(filename, Utils::toSfmlRect(area));
}

sfml_Texture::~sfml_Texture() {
	Log::Inst()->PutMessage("~Texture: " + _name);
}
