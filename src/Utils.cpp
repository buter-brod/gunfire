#include "Utils.h"

#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>

static const float RANDOM_STRENGTH = 5000.f;

namespace Utils {

	time_us getTime() {
		
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
		
		return us;
	}

	bool rndYesNo() {
		return rnd0xi(2) == 0;
	}
	
	const std::string& rndStr(const std::vector<std::string>& vec) {

		if (vec.empty()) {
			static const std::string errorStr = "";
			return errorStr; // sad but error.
		}

		return vec[Utils::rnd0xi((unsigned int)vec.size())];
	}

	size_t rnd() {

		static std::default_random_engine rng(std::random_device{}());
		static std::uniform_real_distribution<float> dist(0, RANDOM_STRENGTH);
		return (size_t)dist(rng);
	}

	float rnd01() { return float(rnd()) / RANDOM_STRENGTH; }
	float rnd0xf(float x) { return rnd01() * x; }
	unsigned int rnd0xi(unsigned int x) { return rnd() % x; }

	float rndfMinMax(float min, float max) { return min + rnd01() * (max - min); }

	
	float dt(const time_us first, const time_us second) {

		if (first < second) {
			return 0; // very sad, but error.
		}

		const auto usDelta = first - second;
		float dtSeconds = float(usDelta) / 1000.f / 1000.f;
		return dtSeconds;
	}
	

	float dt(const float first, const float second) {

		if (first < second) {
			return 0; // very sad, but error.
		}

		const auto usDelta = first - second;
		const float dtSeconds = usDelta;
		return dtSeconds;
	}

	std::string fToString(const float val, unsigned int signs) {
		std::stringstream ss;
		ss << std::fixed << std::setprecision(signs) << val;
		return ss.str();
	}

	std::string toString(const size_t i) {
		std::stringstream ss;
		ss << i;
		return ss.str();
	}
}
