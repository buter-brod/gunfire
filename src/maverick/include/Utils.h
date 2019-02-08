#pragma once

#include <string>
#include <vector>

// time in microseconds since epoch
typedef unsigned long long time_us;

namespace Utils {
	float rnd01();
	float rnd0xf(float x);
	unsigned int rnd0xi(unsigned int x);
	bool  rndYesNo();
	float rndfMinMax(float min, float max);

	/*
	rnd01  is random value 0..1
	rnd0xf  is random value 0..x
	rnd0xi is random integer value 0..x
	*/

	time_us getTime();

	float dt(const time_us first, const time_us second);
	float dt(const float first, const float second);

	struct Color {

		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : _r(r), _g(g), _b(b), _a(a) {};
		Color(unsigned char r, unsigned char g, unsigned char b) : _r(r), _g(g), _b(b), _a(255) {};

		unsigned char _r{ 0 };
		unsigned char _g{ 0 };
		unsigned char _b{ 0 };
		unsigned char _a{ 0 };
	};

	const std::string& rndStr(const std::vector<std::string>& vec);

	std::string toString(const size_t i);
	std::string fToString(const float val, unsigned int signs = 2);
}
