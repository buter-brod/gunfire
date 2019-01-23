#include "Utils.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

namespace Utils {
	sf::IntRect toSfmlRect(const Rect& rect);
	sf::Color toSfmlColor(const Color& clr);
}