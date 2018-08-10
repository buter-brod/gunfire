#include "Graphics.h"
#include "Log.h"

const sf::Texture* Texture::getTex() const { 
	return &_tex; 
}

bool Texture::loadFromFile(const std::string& filename, const Rect& area) {

	_name = filename;
	return _tex.loadFromFile(filename, area);
}

Texture::~Texture() {
	Log::Inst()->PutMessage("~Texture: " + _name);
}


