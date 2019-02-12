#pragma once

#include <memory>
#include <map>

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
	std::map<std::string, std::string> _values;

	static std::shared_ptr<Config>& instancePtr();
};


