#pragma once

#include <string>
#include <time.h>

namespace Utils {
	float rnd01();
	float rnd0xf(float x);
	int   rnd0xi(int x);
	float rndfMinMax(float min, float max);

	/*
	rnd01  is random value 0..1
	rnd0xf  is random value 0..x
	rnd0xi is random integer value 0..x
	*/

	float dt(clock_t first, clock_t second);

	class Point {
	public:
		Point() : _x(0.f), _y(0.f) {}
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

		Point normalized() const;
		float len() const;

		float& X() { return _x; }
		float& Y() { return _y; }

		float getX() const { return _x; }
		float getY() const { return _y; }

		int i_X() const { return int(_x); }
		int i_Y() const { return int(_y); }

		std::string str() const;

	protected:
		float _x, _y;
	};

	std::string toString(const size_t i);
}

typedef Utils::Point Point;
typedef Utils::Point Size;
