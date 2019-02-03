#include "Utils.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Rect.h"

namespace Utils {
	Rect fromSfmlRect(const sf::IntRect& intRect);
	sf::IntRect toSfmlRect(const Rect& rect);
	sf::Color toSfmlColor(const Color& clr);
	sf::Vector2f toSfmlVector2f(const Point& pt);
}