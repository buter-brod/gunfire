#pragma once

#include <string>
#include <vector>

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

		bool operator==(const Point& other) const {
			return
				_x == other._x &&
				_y == other._y;
		}

		void rotate(const Point& pivot, const float angleDeg);

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
		float _x{ 0.f };
		float _y{ 0.f };
	};

	struct Rect {
		Rect() {}
		Rect(const Point& p1, const Point& p2) : _origin(p1), _size(p2) {}
		Rect(const float v1, const float v2, const float v3, const float v4) : _origin({ v1, v2 }), _size({v3, v4}) {}

		bool operator !=(const Rect& other) const {
			const bool equal = (*this == other);
			return !equal;
		}

		bool operator ==(const Rect& other) const{
			return
				(_origin == other._origin) &&
				(_size == other._size);
		}

		Point _origin;
		Point _size;
	};

	std::string toString(const size_t i);
}

typedef Utils::Point Point;
typedef Utils::Point Size;
