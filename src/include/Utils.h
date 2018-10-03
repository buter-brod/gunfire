#pragma once

#include <string>

typedef unsigned long IDType;

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

	float dt(time_us first, time_us second);

	class Point {
	public:
		Point() noexcept : _x(0.f), _y(0.f)  {}
		Point(float x, float y) : _x(x), _y(y) {}

		Point operator+ (const Point& p) const;
		Point operator- (const Point& p) const;
		Point operator* (const Point& p) const;
		Point operator/ (const Point& p) const;

		Point operator-() const;
		void  operator-=(const Point& p);
		void  operator+=(const Point& p);

		void  operator*=(const Point& p);
		void  operator/=(const Point& p);

		void  operator*=(float val);
		Point operator* (float val) const;
		Point operator/ (float val) const;

		void rotate(const Point& pivot, const float angle);

		Point normalized() const;
		float len() const;

		float& X() { return _x; }
		float& Y() { return _y; }

		float getX() const { return _x; }
		float getY() const { return _y; }

		int i_X() const { return int(_x); }
		int i_Y() const { return int(_y); }

		std::string str() const;
		std::string strInt() const;

	protected:
		float _x, _y;
	};

	std::string toString(const size_t i);
}

typedef Utils::Point Point;
typedef Utils::Point Size;
