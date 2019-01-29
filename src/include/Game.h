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

	GameObjectPtr GetObject(const IDType id) const;

	float GetSimulationTime() const { return _simulationTime; };

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

	unsigned int _frags{ 0 };

	typedef std::deque<GameObjectPtr> ObjectsArr;

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _effectObjects;

	/* --- gameplay specific END ---*/

	bool _paused{ false };
	float _simulationTime{ 0.f };
	float _simulationTimeAcc{ 0.f };

	std::unordered_map<IDType, GameObjectWPtr> _allObjects;
	
	void update(const float dt);
	float getTimeRemain() const;

	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool isObjectObsolete(GameObjectPtr objPtr);
	bool checkObjectsObsolete(ObjectsArr& arr);

	void checkAllObjectsObsolete();
	void updateAllObjects(const float dt);

	IDType _nextID{ 1 };
};



