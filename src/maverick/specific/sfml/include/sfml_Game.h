#pragma once
#include "Game.h"

namespace sf {
	class RenderWindow;
}

class sfml_Game : virtual public Game {

public:
	explicit sfml_Game(sf::RenderWindow*);
	~sfml_Game();

	virtual void Init() override;
	virtual void Draw() override;

protected:
	virtual bool update(const float dt) override;
	virtual void drawSpecific();

	bool drawObject(GameObjectPtr obj);

	sf::RenderWindow* getRenderWindow() { return _renderWindow; }

private:
	sf::RenderWindow* _renderWindow{ nullptr };
};