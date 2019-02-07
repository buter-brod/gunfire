#pragma once
#include "Utils.h"
#include "Point.h"

#include <deque>
#include <unordered_map>

class GameObject;
class SoundManager;

typedef std::shared_ptr<GameObject> GameObjectPtr;
typedef std::weak_ptr<GameObject> GameObjectWPtr;
typedef std::weak_ptr<SoundManager> SoundManagerWPtr;

class Game {
public:
	Game();
	~Game();

	virtual void Init();
	virtual void Draw() = 0;
	virtual bool Update(const float dt);

	virtual void OnCursorMoved(const Point& pt) = 0;
	virtual void OnCursorClicked(const Point& pt) = 0;

	Size GetSize() const;

	bool GetPaused() const;
	bool SetPaused(const bool paused);
	IDType Game::newID();

	GameObjectPtr GetObject(const IDType id) const;
	float GetSimulationTime() const { return _simulationTime; };

	typedef std::unordered_map<IDType, GameObjectWPtr> ObjectsMap;

protected:
	typedef std::deque<GameObjectPtr> ObjectsArr;

	virtual bool update(const float dt);
	virtual bool updateSpecific(const float dt);

	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);

	bool addObjectWeak(GameObjectPtr objPtr);

	virtual bool isObjectObsolete(GameObjectPtr objPtr);
	bool checkObjectsObsolete(ObjectsArr& arr);

	virtual void checkAllObjectsObsolete();
	void updateAllObjects(const float dt);

	float getSimulationTime() const { return _simulationTime; }

	const ObjectsMap& getAllObjects() const { return _allObjects; }

private:
	bool _paused{ false };
	float _simulationTime{ 0.f };
	float _simulationTimeAcc{ 0.f };
	ObjectsMap _allObjects;
	SoundManagerWPtr _soundMgr;
	IDType _nextID{ 1 };
};



