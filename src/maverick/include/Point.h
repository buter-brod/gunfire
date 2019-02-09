#pragma once
#include <string>

class Point {
public:
	Point() noexcept : _x(0.f), _y(0.f) {}
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

	bool isEmpty() const;

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

typedef Point Size;