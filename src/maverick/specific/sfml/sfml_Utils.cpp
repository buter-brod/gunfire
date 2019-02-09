#include "sfml_Utils.h"
#include "Point.h"

namespace Utils {
	sf::IntRect toSfmlRect(const Rect& rect) {
		return {
			(int)rect._origin.getX(),
			(int)rect._origin.getY(),
			(int)rect._size.getX(),
			(int)rect._size.getY() };
	}

	Rect fromSfmlRect(const sf::IntRect& intRect) {
		return {
			{ (float)intRect.left,  (float)intRect.top },
			{ (float)intRect.width, (float)intRect.height } };
	}

	sf::Color toSfmlColor(const Color& clr) {
		return { clr._r, clr._g, clr._b, clr._a };
	}

	sf::Vector2f toSfmlVector2f(const Point& pt) {
		return { pt.getX(), pt.getY() };
	}
}

