#include "Point.h"

constexpr float pi = 3.1415926f;

void Point::rotate(const Point& pivot, const float angleDeg) {

	*this -= pivot;

	const float angle = angleDeg / 180.f * float(pi);

	const float c = cos(angle);
	const float s = sin(angle);

	const Point posNew{
		 _x * c - _y * s,
		 _x * s + _y * c
	};

	*this = posNew;
	*this += pivot;
}

bool Point::isEmpty() const {
	return (_x == 0.f && _y == 0.f);
}

std::string Point::str() const {
	return std::string("(") + std::to_string(_x) + ", " + std::to_string(_y) + ")";
}

std::string Point::strInt() const {
	return std::string("(") + std::to_string(int(_x)) + ", " + std::to_string(int(_y)) + ")";
}

Point Point::operator/(const float val) const {
	return { _x / val, _y / val };
}

Point Point::operator*(const float val) const {
	return { _x * val, _y * val };
}

Point Point::operator+(const Point& p) const {
	return { _x + p.getX(), _y + p.getY() };
}

Point Point::operator*(const Point& p) const {
	return { _x * p.getX(), _y * p.getY() };
}

Point Point::operator/(const Point& p) const {
	return { _x / p.getX(), _y / p.getY() };
}

Point Point::operator-(const Point& p) const {
	return { _x - p.getX(), _y - p.getY() };
}

void Point::operator+=(const Point& p) {
	_x += p.getX();
	_y += p.getY();
}

void Point::operator-=(const Point& p) {
	_x -= p.getX();
	_y -= p.getY();
}

Point Point::operator-() const {
	return { -getX(), -getY() };
}

void Point::operator*=(const Point& p) {
	_x *= p.getX();
	_y *= p.getY();
}

void Point::operator/=(const Point& p) {
	_x /= p.getX();
	_y /= p.getY();
}

void Point::operator*=(float val) {
	_x *= val;
	_y *= val;
}

Point Point::normalized() const {
	return (*this * (1.0f / len()));
}

float Point::len() const {

	const float sq = (_x * _x + _y * _y);
	const float ln = sqrt(sq);

	return ln;
}