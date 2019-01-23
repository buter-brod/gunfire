#pragma once
#include "Utils.h"
#include <map>
#include "GameObject.h"
#include <deque>
#include "Text.h"
#include "Player.h"

class Sprite;

class Game : public std::enable_shared_from_this<Game> {
public:
	Game();
	~Game();

	void Init();
	void Draw();
	void Update(const float dt);

	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);
	
	Size GetSize() const;

	bool GetPaused() const;
	bool SetPaused(const bool paused);

	float GetSimulationTime() const { return _simulationTime; };

	IDType Game::newID();

protected:

	/* --- gameplay specific BEGIN ---*/

	void update(const float dt);

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

	bool _paused{ false };

	FontPtr _font;

	float _simulationTime{ 0.f };
	float _simulationTimeAcc{ 0.f };

	unsigned int _frags{ 0 };

	typedef std::deque<GameObjectPtr> ObjectsArr;

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _effectObjects;

	/* --- gameplay specific END ---*/
	
	float Game::getTimeRemain() const;

	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool isObjectObsolete(GameObjectPtr objPtr);
	void checkObjectsObsolete(ObjectsArr& arr);

	IDType _nextID{ 1 };
};



