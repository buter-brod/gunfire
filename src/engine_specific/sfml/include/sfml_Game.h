#include "Game.h"

namespace sf {
	class Font;
	class Text;
	class RenderWindow;
}

typedef std::shared_ptr<sf::Font> FontPtr;
typedef std::shared_ptr<sf::Text> TextPtr;

class sfml_Game : public Game, public std::enable_shared_from_this<sfml_Game> {
public:
	sfml_Game(sf::RenderWindow*);
	~sfml_Game();

	virtual void Init() override;

	void Draw() override;

protected:
	virtual bool update(const float dt) override;
	virtual void initText() override;
	virtual void updateText() override;

private:
	sf::RenderWindow* _renderWindow{ nullptr };

	TextPtr _timerTxt;
	TextPtr _scoreTxt;
	TextPtr _gameOverText;

	FontPtr _font;
};