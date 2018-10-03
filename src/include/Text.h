#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include <memory>

typedef sf::Font Font;
typedef sf::Text Text;


typedef std::shared_ptr<Font> FontPtr;
typedef std::shared_ptr<Text> TextPtr;
