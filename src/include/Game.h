#pragma once
#include "Utils.h"
#include <map>
#include "GameObject.h"
#include <deque>
#include "Text.h"
#include "Player.h"

class Sprite;

namespace sf {
	class RenderWindow;
}

class Game {
public:
	Game();
	~Game();

	void Init();
	void Draw(sf::RenderWindow* wnd);
	void Update(const float dt);

	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);
	
	Size GetSize() const;

	IDType Game::newID();

protected:

	/* --- gameplay specific BEGIN ---*/

	void tryShoot(const Point& whereTo);
	void checkSpawn();

	void checkCollisions();
	void onCollision(GameObjectPtr bullet, GameObjectPtr enemy);

	void initText();
	void initSound();
	void updateText();

	GameObjectPtr _bgObject;
	PlayerPtr _playerObj;

	TextPtr _timerTxt;
	TextPtr _scoreTxt;
	TextPtr _gameOverText;

	FontPtr _font;

	time_us _startTimetamp{ 0 };

	unsigned int _frags{ 0 };

	typedef std::deque<GameObjectPtr> ObjectsArr;

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _effectObjects;

	/* --- gameplay specific END ---*/
	
	time_us Game::getTimeRemain() const;

	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool isObjectObsolete(GameObjectPtr objPtr);
	void checkObjectsObsolete(ObjectsArr& arr);

	IDType _nextID{ 1 };
};



