#include "sfml_Utils.h"

namespace Utils {
	sf::IntRect toSfmlRect(const Rect& rect) {
		return sf::IntRect(
			(int)rect._origin.getX(),
			(int)rect._origin.getY(),
			(int)rect._size.getX(),
			(int)rect._size.getY());
	}

	sf::Color toSfmlColor(const Color& clr) {
		return sf::Color(clr._r, clr._g, clr._b, clr._a);
	}
}

