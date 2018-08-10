#pragma once
#include <memory>
#include <SFML/Graphics.hpp>

class TextureAtlas;

typedef sf::IntRect Rect;

class Texture {

public:
	const sf::Texture* getTex() const;
	bool loadFromFile(const std::string& filename, const Rect& area = Rect());
	~Texture();

private:
	sf::Texture _tex;
	std::string _name;
};

class Sprite {

public:
	Sprite(const Texture& texture, const Rect& rectangle) : _sprite(*texture.getTex(), rectangle) {}
	Sprite(const Texture& texture) : _sprite(*texture.getTex()) {}
	sf::Sprite* getSpr() { return &_sprite; }

private:
	sf::Sprite _sprite;
};

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWPtr;

struct TextureRect {
	TextureWPtr texturePtr;
	Rect rect;
};