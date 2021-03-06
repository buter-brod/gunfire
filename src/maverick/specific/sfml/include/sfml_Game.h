#pragma once
#include "Game.h"

namespace sf {
	class RenderWindow;
}

class sfml_Game : public Game {

public:
	explicit sfml_Game(sf::RenderWindow*);
	~sfml_Game();

	virtual void Init() override;
	virtual void Draw() override;

protected:
	virtual bool update(const float dt) override;
	bool drawObject(const GameObject* obj) const;

	sf::RenderWindow* getRenderWindow() const { return _renderWindow; }

private:
	sf::RenderWindow* _renderWindow{ nullptr };
};