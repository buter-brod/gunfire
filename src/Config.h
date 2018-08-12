#pragma once
#include "Utils.h"
#include <SFML/Graphics/Color.hpp>
#include <vector>

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

	static const std::string bulletName = "bottle";
	static const std::string bulletSpr = "bottle_fire.png";
	static const unsigned int bulletAnimFramesCount = 2;
	static const unsigned int bulletAnimFPS = 5;

	static const std::string boomSpr = "boom.png";
	static const unsigned int boomAnimFramesCount = 10;
	static const unsigned int boomAnimFPS = 30;

	static const std::vector<std::string> enemySounds = { "police0.ogg", "police1.ogg", "police2.ogg", "police3.ogg" };

	static const std::vector<std::string> boomSounds = { "boom0.ogg", "boom1.ogg" };
	static const std::string ambientSound = "ambient.ogg";

	static const std::string musicTrack = "music.ogg";

	static const std::string readySound = "lighter.ogg";
	static const std::string throwSound = "throw.ogg";

	static const std::string playerName = "fox";
	static const std::string playerSpr = "fox.png";
	static const std::string playerFireSpr = "fox_throw.png";
	static const std::string playerCooldownSpr = "fox_empty.png";

	static const float throwDuration{ 0.5f };
	static const float cooldown{ 1.5f };
	
	static const std::string bgName = "background";
	static const std::string bgSprName = "background.png";

	static const std::string enemyName = "police";
	static const std::string enemySpr = "police.png";
	static const unsigned int enemyAnimFramesCount = 2;
	static const unsigned int enemyAnimFPS = 5;
	static const unsigned int enemyCount = 3;

	static const float boomAcceleration = 300.f;
	static const float boomAngleSpeed = 30.f;

	static const std::string idleStateName = "idle";
	static const std::string cooldownStateName = "cooldown";
	static const std::string shootingStateName = "shooting";
	static const std::string boomStateName = "boom";
	static const std::string deadStateName = "dead";

	static const float enemyPositionGapX = 100;

	static const float enemyPositionGapYMin = 80;
	static const float enemyPositionGapYMax = 280;

	static const float enemySpeedMin = 50;
	static const float enemySpeedMax = 350;

	static const float playerPositionGapY = 145;
	static const float bulletPositionGapY = 290;

	static const float bulletSpeed = 200;

	static const float bulletAngleSpeedMin = 30;
	static const float bulletAngleSpeedMax = 360;

	static const float bulletSpeedMin = 100;
	static const float bulletSpeedMax = 300;
}

