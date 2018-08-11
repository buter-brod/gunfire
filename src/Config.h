#pragma once
#include "Utils.h"
#include <SFML/Graphics/Color.hpp>

namespace Config {

	inline std::string getResourceDir() {
		return "../Resources/";
	}

	static const unsigned int simulationFPS = 60;

	static const Size gameSize(1024, 768);
	static const Size windowSize = gameSize;
	static const std::string restartImgFile = "reset.png";

	static const std::string appTitle = "gunfire";
	static const sf::Color btnNotHovered(230, 230, 230);

	static const std::string bulletSpr = "bottle_fire.png";
	static const unsigned int bulletAnimFramesCount = 2;
	static const unsigned int bulletAnimFPS = 5;

	static const std::string boomSpr = "boom.png";
	static const unsigned int boomAnimFramesCount = 10;
	static const unsigned int boomAnimFPS = 10;

	static const std::string playerSpr = "fox.png";
	static const std::string playerFireSpr = "fox_throw.png";


	static const std::string enemySpr = "plane.png";
	static const unsigned int enemyCount = 3;


	// gameplay-specific


	static const float enemyPositionGapX = 100;

	static const float enemyPositionGapYMin = 80;
	static const float enemyPositionGapYMax = 280;

	static const float enemySpeedMin = 50;
	static const float enemySpeedMax = 350;

	static const float playerPositionGapY = 115;
	static const float bulletPositionGapY = 270;

	static const float bulletSpeed = 200;

	static const float bulletAngleSpeedMin = 30;
	static const float bulletAngleSpeedMax = 360;

	static const float bulletSpeedMin = 100;
	static const float bulletSpeedMax = 300;


}

