#include "Rect.h"

bool Rect::operator !=(const Rect& other) const {
	const bool equal = (*this == other);
	return !equal;
}

bool Rect::operator ==(const Rect& other) const {
	return
		(_origin == other._origin) &&
		(_size == other._size);
}

bool Rect::isEmpty() const {
	return _origin.isEmpty() && _size.isEmpty();
}