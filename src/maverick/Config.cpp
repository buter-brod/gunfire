#include "Config.h"
#include "Log.h"
#include "CfgStatic.h"

#include <fstream>
#include <algorithm>

std::shared_ptr<Config>& Config::instancePtr() {
	static std::shared_ptr<Config> instancePtr{ nullptr };
	return instancePtr;
}

Config::Config() {
	init();
}

bool Config::hasValue(const std::string& key) const {
	
	const bool found = _values.find(key) != _values.end();
	return found;
}

std::string Config::getString(const std::string& key) const {

	const auto valIt = _values.find(key);

	if (valIt == _values.end()) {
		Log::Inst()->PutErr("Config::getString error, unable to read key " + key);
		return "";
	}
	
	return valIt->second;
}

int Config::getInt(const std::string& key) const {

	const std::string& valStr = getString(key);
	try {
		const int val = std::stoi(valStr);
		return val;
	}
	catch (...) {}

	Log::Inst()->PutErr("Config::getInt error, unable to read key " + key);

	return badNum;
}

float Config::getFloat(const std::string& key) const {

	const std::string& valStr = getString(key);
	try {
		const float val = std::stof(valStr);
		return val;
	}
	catch (...) {}

	Log::Inst()->PutErr("Config::getFloat error, unable to read key " + key);

	return float(badNum);
}

std::string & ltrim(std::string & str) {
	const auto it2 = std::find_if(str.begin(), str.end(), [](char ch) { return ch != ' ';});
	str.erase(str.begin(), it2);
	return str;
}

std::string & rtrim(std::string & str) {
	auto it1 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return ch != ' ';});
	str.erase(it1.base(), str.end());
	return str;
}

std::string trimmed(std::string const & str) {
	auto s = str;
	return ltrim(rtrim(s));
}

void Config::init() {

	std::ifstream input(CfgStatic::configFile);
	std::string line;

	while (std::getline(input, line)) {

		const auto eqPos = line.find('=');

		if (line.empty() || line[0] == '#') {
			continue;
		}

		if (eqPos == 0 || eqPos == std::string::npos || eqPos == line.size() - 1) {

			Log::Inst()->PutErr("Config::init error, wrong string " + line);
			continue;
		}

		const std::string key = trimmed(line.substr(0, eqPos));
		const std::string val = trimmed(line.substr(eqPos + 1));

		_values[key] = val;
	}
}

Config* Config::Inst() {

	auto& ptr = instancePtr();
	if (ptr == nullptr) {
		ptr.reset(new Config());

		ptr->init();
	}

	return ptr.get();
}

