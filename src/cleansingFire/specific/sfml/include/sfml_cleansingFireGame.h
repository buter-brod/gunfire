#pragma once
#include "CleansingFireGame.h"
#include "sfml_Game.h"

#ifdef _WINDOWS
#pragma warning(disable: 4250)
// MSVS displays annoying warning even when with valid virtual inheritance usage
#endif

namespace sf {
	class Font;
	class Text;
}

typedef std::shared_ptr<sf::Font> FontPtr;
typedef std::shared_ptr<sf::Text> TextPtr;

class sfml_cleansingFireGame : public CleansingFireGame, public sfml_Game {
public:
	explicit sfml_cleansingFireGame(sf::RenderWindow*);
	~sfml_cleansingFireGame();

	virtual void Init() override;

protected:
	virtual void initText() override;
	virtual void updateText() override;
	virtual void drawSpecific() override;

private:
	TextPtr _timerTxt;
	TextPtr _scoreTxt;
	TextPtr _gameOverText;

	FontPtr _font;
};