#pragma once
#include "Point.h"
#include "Utils.h"

#include "CfgStatic.h"
#include <vector>

//#define RELEASE_BUILD

namespace CfgStatic {

	// sizes

	static const Size windowSize = gameSize;

	// object sizes

	static const Size playerSize(330, 287);
	static const Size bulletSize(79, 77);
	static const Size enemySize(224, 137);

	// files

	static const std::string fontName = getResourceDir() + "the unseen.ttf";

	// sprites
	
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
	
	static constexpr unsigned int bulletAnimFPS = 5;
	static constexpr unsigned int   boomAnimFPS = 20;
	static constexpr unsigned int  enemyAnimFPS = 5;

	// animation

	static constexpr unsigned int bulletAnimFramesCount = 2;
	static constexpr unsigned int boomAnimFramesCount   = 10;
	static constexpr unsigned int enemyAnimFramesCount  = 2;
	
	// time

	static constexpr float outroEffectDuration = 3.f;
	static constexpr float smokeLifetime{ 1.f };
	static constexpr float boomLifetime{ 0.15f };

	// text

	static const std::string appTitle = "cleansingFire";
	static const std::string timerTxt = "Time before arrest: ";
	static const std::string scoreTxt = "Score: ";
	
	// object names
	
	static const std::string bulletName = "bottle";
	static const std::string playerName = "fox";
	static const std::string bgName     = "background";
	static const std::string enemyName  = "police";

	// speed

	static constexpr float boomAcceleration    = 300.f;
	static constexpr float boomAngleSpeed      = 30.f;

	static constexpr float bulletAngleSpeedMin = 30.f;
	static constexpr float bulletAngleSpeedMax = 360.f;
	static constexpr float bulletSpeedMin      = 100.f;
	static constexpr float bulletSpeedMax      = 300.f;
	static constexpr float bulletSpeed         = 200.f;

	static constexpr float smokeFadeIn         = 0.15f;
	static constexpr float smokeFadeOut        = 0.85f;
	static constexpr float smokeSpeed          = 20.f;
	static constexpr float smokeEmission       = 45.f;

	// positions

	static constexpr float enemyPositionGapX    = 100;
	static constexpr float enemyPositionGapYMin = 80;
	static constexpr float enemyPositionGapYMax = 280;

	static constexpr float scorePositionX       = 850;
	static constexpr float scorePositionY       = 700;

	static constexpr float timerPositionX       = 50;
	static constexpr float timerPositionY       = 700;

	static constexpr float smokeMaxDeflect  = 15.f;

	static constexpr float collideDistCoeff = 4.f;

	//states

	static const std::string cooldownStateName = "cooldown";
	static const std::string shootingStateName = "shooting";
	static const std::string     boomStateName = "boom";
	static const std::string   boomedStateName = "boomed";
	static const std::string    dyingStateName = "dying";
	static const std::string  leavingStateName = "leaving";
	
}

