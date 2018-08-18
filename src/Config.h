#pragma once
#include "Utils.h"
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

class Config{

public:
	static Config* Inst();
	static const int badNum{ INT_MIN };

	std::string getString(const std::string& key) const;
	int getInt(const std::string& key) const;
	float getFloat(const std::string& key) const;

private:
	Config();
	void init();
	std::unordered_map <std::string, std::string> _values;

	static std::shared_ptr<Config>& instancePtr();
};

namespace CfgStatic {

	// sizes

	static const Size gameSize(1024, 768);
	static const Size windowSize = gameSize;

	// files

	inline std::string getResourceDir() {
		return "../Resources/";
	}

	static const std::string configFile = getResourceDir() + "config.cfg";
	static const std::string fontName   = getResourceDir() + "the unseen.ttf";

	static const std::string pixelizeShader = "pixelize";

	static const std::string& logFile = ""; // "log.txt"; // log file disabled

	static const std::string vertExt = ".vert";
	static const std::string fragExt = ".frag";

	// sprites

	static const std::string atlasPng  = "atlas.png";
	static const std::string atlasMtpf = "atlas.mtpf";

	static const std::string restartImgFile    = "reset.png";
	static const std::string bulletSpr         = "bottle_fire.png";
	static const std::string boomSpr           = "boom.png";
	static const std::string playerSpr         = "fox.png";
	static const std::string playerFireSpr     = "fox_throw.png";
	static const std::string playerCooldownSpr = "fox_empty.png";
	static const std::string bgSprName         = "background.png";
	static const std::string enemySpr          = "police.png";

	//sounds

	static const std::vector<std::string> enemySounds = { "police0.ogg", "police1.ogg", "police2.ogg", "police3.ogg" };
	static const std::vector<std::string> boomSounds  = { "boom0.ogg", "boom1.ogg" };

	static const std::string ambientSound = "ambient.ogg";
	static const std::string musicTrack   = "music.ogg";
	static const std::string readySound   = "lighter.ogg";
	static const std::string throwSound   = "throw.ogg";

	//colors 

	static const sf::Color btnNotHovered(230, 230, 230);
	static const sf::Color scoreColor(0, 255, 0);

	// FPS
	
	static const unsigned int simulationFPS = 60;
	static const unsigned int bulletAnimFPS = 5;
	static const unsigned int boomAnimFPS   = 30;
	static const unsigned int enemyAnimFPS  = 5;
	const unsigned int fpsLogFramesCap = 50000;

	// animation

	static const unsigned int bulletAnimFramesCount = 2;
	static const unsigned int boomAnimFramesCount   = 10;
	static const unsigned int enemyAnimFramesCount  = 2;
	
	// time

	static const float throwDuration{ 0.5f };
	static const float cooldown{ 1.5f };

	// text

	static const std::string appTitle = "gunfire";
	static const std::string timerTxt = "Time left: ";
	static const std::string scoreTxt = "Score: ";
	
	// object names
	
	static const std::string bulletName = "bottle";
	static const std::string playerName = "fox";
	static const std::string bgName     = "background";
	static const std::string enemyName  = "police";

	// speed

	static const float boomAcceleration    = 300.f;
	static const float boomAngleSpeed      = 30.f;
	static const float bulletAngleSpeedMin = 30.f;
	static const float bulletAngleSpeedMax = 360.f;
	static const float bulletSpeedMin      = 100.f;
	static const float bulletSpeedMax      = 300.f;
	static const float bulletSpeed         = 200.f;
	static const float pixelizeSpeed       = 10.f;
	static const float pixelizeCoeffMax    = 15.f;

	// positions

	static const float enemyPositionGapX    = 100;
	static const float enemyPositionGapYMin = 80;
	static const float enemyPositionGapYMax = 280;
	static const float playerPositionGapY   = 145;
	static const float bulletPositionGapY   = 290;
	static const float scorePositionX       = 850;
	static const float scorePositionY       = 700;
	const float timerPositionX              = 80;
	const float timerPositionY              = 700;

	//states

	static const std::string idleStateName     = "idle";
	static const std::string cooldownStateName = "cooldown";
	static const std::string shootingStateName = "shooting";
	static const std::string boomStateName     = "boom";
	static const std::string enemyDStateName   = "dying";
	static const std::string deadStateName     = "dead";
}

