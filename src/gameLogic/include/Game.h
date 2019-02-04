#pragma once
#include "Utils.h"
#include "Point.h"

#include <deque>
#include <unordered_map>

class GameObject;
class Player;
class SoundManager;

typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;

class Game {
public:
	Game();
	~Game();

	virtual void Init();
	virtual void Draw() = 0;
	bool Update(const float dt);

	void OnCursorMoved(const Point& pt);
	void OnCursorClicked(const Point& pt);
	
	Size GetSize() const;

	bool GetPaused() const;
	bool SetPaused(const bool paused);

	GameObjectPtr GetObject(const IDType id) const;

	float GetSimulationTime() const { return _simulationTime; };

	IDType Game::newID();

	typedef std::unordered_map<IDType, GameObjectWPtr> ObjectsMap;

protected:
	/* --- gameplay specific BEGIN ---*/

	bool tryShoot(const Point& whereTo);
	void checkSpawn();

	void checkCollisions();
	void onCollision(GameObjectPtr bullet, GameObjectPtr enemy);

	virtual void initText();
	virtual void initSound();
	virtual void updateText();

	unsigned int getFrags() const { return _frags; }

	GameObjectPtr getBGObject() const { return _bgObject; }
	PlayerPtr getPlayerObject() const { return _playerObj; }

	typedef std::deque<GameObjectPtr> ObjectsArr;

	const ObjectsArr& getEnemyObjects() const  { return _enemyObjects; }
	const ObjectsArr& getBulletObjects() const { return _bulletObjects; }

private:

	GameObjectPtr _bgObject;
	PlayerPtr _playerObj;

	struct {
		bool requested{ false };
		Point targetPt;
	} _shootRequest;
	
		
	unsigned int _frags{ 0 };
	float _bonusTime{ 0.f };

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;

	/* --- gameplay specific END ---*/

protected:
	virtual bool update(const float dt);
	float getTimeRemain() const;

	bool addObject(GameObjectPtr objPtr);
	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool isObjectObsolete(GameObjectPtr objPtr);
	bool checkObjectsObsolete(ObjectsArr& arr);

	void checkAllObjectsObsolete();
	void updateAllObjects(const float dt);

	const ObjectsMap& getAllObjects() const { return _allObjects; }

private:

	bool _paused{ false };
	float _simulationTime{ 0.f };
	float _simulationTimeAcc{ 0.f };

	ObjectsMap _allObjects;

	SoundManagerWPtr _soundMgr;

	IDType _nextID{ 1 };
};



