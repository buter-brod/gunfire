#pragma once
#include "Point.h"

//#define RELEASE_BUILD

namespace CfgStatic {
	
	// Sizes

	static const Size gameSize(1024, 768);

	// Files
	
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
	
	static const std::string pixelizeShader = "pixelize";
	
	static const std::string& logFile = ""; // "log.txt"; // log file disabled

	static const std::string vertExt = ".vert";
	static const std::string fragExt = ".frag";
	
	// sprites

	static const std::string atlasPng  = "atlas.png";
	static const std::string atlasMtpf = "atlas.mtpf";
	
	// FPS
	
	static constexpr unsigned int simulationFPS = 60;
	static constexpr float fpsLogTime = 3.f;
	
	// States
	
	static const std::string idleStateName = "idle";
	static const std::string deadStateName = "dead";

	// Speed

	static constexpr float pixelizeDuration = 4.f;
	static constexpr float pixelizeCoeffMax = 15.f;

}


