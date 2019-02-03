#pragma once
#include <string>
#include <set>

class Log {

public:

	enum class Channel {
		ERROR,
		MISC,
		VERBOSE
	};

	static Log* Inst();

	void PutErr(const std::string& msg);
	void PutMessage(const std::string& msg);
	void PutMessage(const Channel ch, const std::string& msg);

protected:

	static void putMsg(const std::string& msg);

	std::set<Channel> getEnabledChannels();

private:

	Log();
};