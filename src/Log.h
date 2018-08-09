#pragma once
#include <string>
#include <set>
#include <iostream>

class Log {

public:

	enum class Channel {
		ERROR,
		MISC,
		VERBOSE
	};

	static Log* inst();

	void PutErr(const std::string& msg);
	void PutMessage(const std::string& msg);
	void PutMessage(const Channel ch, const std::string& msg);

protected:

	static void putMsg(const std::string& msg);

	std::set<std::ostream*>& getStreams();
	std::set<Channel> getEnabledChannels();

private:

	Log();
};