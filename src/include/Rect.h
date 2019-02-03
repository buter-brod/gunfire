#pragma once
#include "Point.h"

struct Rect {
	Rect() {}
	Rect(const Point& p1, const Point& p2) : _origin(p1), _size(p2) {}
	Rect(const float v1, const float v2, const float v3, const float v4) : _origin({ v1, v2 }), _size({ v3, v4 }) {}

	bool operator !=(const Rect& other) const;
	bool operator ==(const Rect& other) const;

	bool isEmpty() const;

	Point _origin;
	Point _size;
};