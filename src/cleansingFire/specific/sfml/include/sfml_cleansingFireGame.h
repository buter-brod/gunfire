#pragma once
#include "sfml_Game.h"

namespace sf {
	class Font;
	class Text;
}

class CFGameplayComponent;
typedef std::shared_ptr<CFGameplayComponent> CFGameplayComponentPtr;

typedef std::shared_ptr<sf::Font> FontPtr;
typedef std::shared_ptr<sf::Text> TextPtr;

class sfml_cleansingFireGame : public sfml_Game{
public:
	explicit sfml_cleansingFireGame(sf::RenderWindow*);
	~sfml_cleansingFireGame();

	virtual void Init() override;

protected:
	virtual void initText();
	virtual void updateText();
	virtual void Draw() override;

	virtual bool update(const float dt) override;

private:
	CFGameplayComponentPtr _gameplayPtr;

	TextPtr _timerTxt;
	TextPtr _scoreTxt;
	TextPtr _gameOverText;

	FontPtr _font;
};