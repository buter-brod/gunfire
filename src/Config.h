#pragma once
#include "Utils.h"
#include <SFML/Graphics/Color.hpp>

namespace Config {

	inline std::string getResourceDir() {
		return "../Resources/";
	}

	static const Size gameSize(1024, 768);
	static const Size windowSize = gameSize;
	static const std::string restartImgFile = "reset.png";

	static const std::string appTitle = "gunfire";
	static const sf::Color btnNotHovered(230, 230, 230);
}

