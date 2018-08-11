#pragma once
#include "Utils.h"
#include <map>
#include "GameObject.h"
#include <deque>

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

	void shoot(const Point& whereTo);
	void spawn();

	GameObjectPtr _bgObject;
	GameObjectPtr _playerObj;

	typedef std::deque<GameObjectPtr> ObjectsArr;

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _effectObjects;

	/* --- gameplay specific END ---*/

	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool isObjectObsolete(GameObjectPtr objPtr);
	void checkObjectsObsolete(ObjectsArr& arr);

	IDType _nextID{ 1 };
};



