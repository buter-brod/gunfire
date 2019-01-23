#include "Texture.h"
#include "Log.h"
#include "sfml_Utils.h"

const sf::Texture* Texture::getTex() const {
	return &_tex;
}

bool Texture::loadFromFile(const std::string& filename, const Utils::Rect& area) {

	_name = filename;
	return _tex.loadFromFile(filename, Utils::toSfmlRect(area));
}

Texture::~Texture() {
	Log::Inst()->PutMessage("~Texture: " + _name);
}
