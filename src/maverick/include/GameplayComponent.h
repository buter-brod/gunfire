#pragma once
#include "MiscForward.h"

#include "Point.h"

#include <functional>
#include <queue>

class GameplayComponent {

public:
	explicit GameplayComponent(const Size& sz);
	virtual ~GameplayComponent() = default;

	const std::string& GetName() const { return _name; }

	virtual void Init();
	virtual void OnCursorMoved(const Point& pt);
	virtual void OnCursorClicked(const Point& pt);

	virtual GameObjectCArrPtrVec GetObjectLists();

	void SetPaused(const bool paused);
	void SetGameServicesPtr(const GameServicesWPtr& gsptr);

	virtual bool Update(const float dt);

protected:
	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr) const;
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr) const;
	bool removeObject(const IDType id, ObjectsArr& arr) const;

	bool checkObjectsObsolete(ObjectsArr& arr);
	void setName(const std::string& name);

	void schedule(std::function<void()> f);

	GameServicesWPtr getGameServicesWPtr() const { return _gameServicesWPtr; }

	float getSimulationTime() const { return _simulationTime; }
	bool getPaused() const { return _paused; }
	const Size& getSize() const;
	virtual GameObjectArrPtrVec getObjectLists();

	IDType newID() const;
	void invalidateObjects() const;

	virtual bool isObjectObsolete(GameObjectPtr objPtr);

private:
	Size _size;
	bool _paused{ false };
	float _simulationTime{ 0.f };
	std::string _name;

	std::queue<std::function<void()> > _scheduled;

	GameServicesWPtr _gameServicesWPtr;
};
