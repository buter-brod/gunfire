#include "Utils.h"
#include <sstream>
#include <random>

static const float RANDOM_STRENGTH = 5000.f;

namespace Utils {

	size_t rnd() {

		static std::default_random_engine rng(std::random_device{}());
		static std::uniform_real_distribution<float> dist(0, RANDOM_STRENGTH);
		return (size_t)dist(rng);
	}

	float rnd01() { return float(rnd()) / RANDOM_STRENGTH; }
	float rnd0xf(float x) { return rnd01() * x; }
	int   rnd0xi(int   x) { return rnd() % x; }

	float rndfMinMax(float min, float max) { return min + rnd01() * (max - min); }

	float dt(clock_t first, clock_t second) {
		return std::abs(float(first) - float(second)) / CLOCKS_PER_SEC;
	}

	std::string toString(const size_t i) {
		std::stringstream ss;
		ss << i;
		return ss.str();
	}

	std::string Point::str() const {
		return std::string("(") + std::to_string(_x) + ", " + std::to_string(_y) + ")";
	}

	Point Point::operator/(float val) const {
		return Point(_x / val, _y / val);
	}

	Point Point::operator*(float val) const {
		return Point(_x * val, _y * val);
	}

	Point Point::operator+(const Point& p) const {
		return Point(_x + p.getX(), _y + p.getY());
	}

	Point Point::operator*(const Point& p) const {
		return Point(_x * p.getX(), _y * p.getY());
	}

	Point Point::operator/(const Point& p) const {
		return Point(_x / p.getX(), _y / p.getY());
	}

	Point Point::operator-(const Point& p) const {
		return Point(_x - p.getX(), _y - p.getY());
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
		return Point(-getX(), -getY());
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
		return sqrt(_x *_x + _y * _y);
	}
}
