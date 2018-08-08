#include <SFML/Graphics.hpp>
#include <iostream>

const unsigned int screenW = 1024;
const unsigned int screenH = 768;

const std::string bgSprName = "clouds.png";

inline std::string getResourceDir() {
	return "../Resources/";
}

int main() {
	
	sf::RenderWindow window(sf::VideoMode(screenW, screenH), "gunfire");

	sf::Texture bgTex;
	if (!bgTex.loadFromFile(getResourceDir() + bgSprName)) {
		std::cout << "ERROR: unable to load clouds.png \n";
	}

	sf::Sprite bgSprite;
	bgSprite.setTexture(bgTex);

	while (window.isOpen())	{
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::Black);
		window.draw(bgSprite);
		window.display();
	}

	return 0;
}



