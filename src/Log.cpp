#include "Log.h"
#include <fstream>
#include "Config.h"

static const bool alwaysToCout = true;

Log* Log::Inst() {
	static Log* instance = new Log();
	return instance;
}

Log::Log() {

	if (!CfgStatic::logFile.empty()) {
		std::ofstream logFileStream(CfgStatic::logFile);

		if (logFileStream.good()) {
			logFileStream << "Game Log: \n";
			logFileStream.close();
		}
		else {
			std::cout << "Log::Log error, unable to open " + CfgStatic::logFile + "\n";
		}
	}
}

std::set<Log::Channel> Log::getEnabledChannels() {

	static std::set<Log::Channel> channels = { 
		Channel::ERROR, 
		Channel::MISC
		//,Channel::VERBOSE
	};

	return channels;
}

void Log::putMsg(const std::string& msg) {
	
	if (CfgStatic::logFile.empty() || alwaysToCout) {
		std::cout << msg;
	}
	
	if (!CfgStatic::logFile.empty()) {
		std::ofstream logFileStream;
		logFileStream.open(CfgStatic::logFile, std::ofstream::out | std::ofstream::app);

		if (logFileStream.good()) {
			logFileStream << msg;
		}
		else {
			std::cout << "Log::putMsg error, unable to open " + CfgStatic::logFile + "\n";
		}
	}
}

void Log::PutErr(const std::string& msg) {
	PutMessage(Channel::ERROR, msg);
}

void Log::PutMessage(const std::string& msg) {
	PutMessage(Channel::MISC, msg);
}

void Log::PutMessage(const Channel ch, const std::string& msg) {

	const auto& enabledChannels = getEnabledChannels();

	if (enabledChannels.count(ch) > 0) {
		const std::string logMsg = "LOG: " + msg + "\n";
		putMsg(logMsg);
	}
}