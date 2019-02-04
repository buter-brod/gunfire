#pragma once
#include "Rect.h"
#include "Utils.h"

#include <vector>
#include <unordered_map>
#include <memory>

//#define RELEASE_BUILD

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

	// object sizes

	static const Size playerSize(330, 287);
	static const Size bulletSize(79, 77);
	static const Size enemySize(224, 137);

	// files

	inline std::string getSoundDir() {
		return "sound/";
	}

	inline std::string getResourceDir() {
#ifdef RELEASE_BUILD
		return "./Resources/";
#else
		return "../Resources/";
#endif
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

	static const std::string smokePng = "smoke.png";

	static const std::string playImgFile       = "play.png";
	static const std::string pauseImgFile      = "pause.png";
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

	static const Utils::Color btnPlayPauseNotHoveredClr(230, 230, 230);
	static const Utils::Color btnNotHoveredClr(230, 230, 230);
	static const Utils::Color scoreClr(0, 255, 0);
	static const Utils::Color smokeClr(192, 192, 192);

	// FPS
	
	static const bool drawFPSUnlimited = true; // if set to false, drawing FPS will be the same as simulationFPS

	static const unsigned int simulationFPS = 60;

	static const unsigned int bulletAnimFPS = 5;
	static const unsigned int   boomAnimFPS = 20;
	static const unsigned int  enemyAnimFPS = 5;

	const unsigned int fpsLogFramesCap = 15000;

	// animation

	static const unsigned int bulletAnimFramesCount = 2;
	static const unsigned int boomAnimFramesCount   = 10;
	static const unsigned int enemyAnimFramesCount  = 2;
	
	// time

	static const float throwDuration{ 0.5f };
	static const float cooldown{ 1.5f };
	static const float smokeLifetime{ 1.f };
	static const float boomLifetime{ 0.15f };

	// text

	static const std::string appTitle = "gunfire";
	static const std::string timerTxt = "Time before arrest: ";
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

	static const float pixelizeSpeed       = 5.f;
	static const float pixelizeCoeffMax    = 15.f;
	
	static const float smokeFadeIn         = 0.15f;
	static const float smokeFadeOut        = 0.85f;
	static const float smokeSpeed          = 20.f;
	static const float smokeEmission       = 45.f;

	// positions

	static const float enemyPositionGapX    = 100;
	static const float enemyPositionGapYMin = 80;
	static const float enemyPositionGapYMax = 280;

	static const float scorePositionX       = 850;
	static const float scorePositionY       = 700;

	const float timerPositionX              = 50;
	const float timerPositionY              = 700;

	static const float smokeMaxDeflect      = 15.f;

	//states

	static const std::string     idleStateName = "idle";
	static const std::string cooldownStateName = "cooldown";
	static const std::string shootingStateName = "shooting";
	static const std::string     boomStateName = "boom";
	static const std::string   boomedStateName = "boomed";
	static const std::string    dyingStateName = "dying";
	static const std::string  leavingStateName = "leaving";
	static const std::string     deadStateName = "dead";
}

