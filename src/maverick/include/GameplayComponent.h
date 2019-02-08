#pragma once
#include "MiscForward.h"

#include "Point.h"
#include <functional>

typedef std::function<IDType()> IDTypeFunc;
typedef std::function<void()> InvalidateObjectsFunc;

class GameplayComponent {

public:
	explicit GameplayComponent(const Size& sz);
	virtual ~GameplayComponent() {}

	const std::string& GetName() const { return _name; }

	virtual void Init();
	virtual void OnCursorMoved(const Point& pt);
	virtual void OnCursorClicked(const Point& pt);

	virtual GameObjectCArrPtrVec GetObjectLists();

	void SetPaused(const bool paused);
	void SetIDFunc(const IDTypeFunc f);

	// who should be informed that object lists were changed
	void SetObjInvalidateFunc(const InvalidateObjectsFunc f);

	virtual bool Update(const float dt);

protected:
	bool addObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(GameObjectPtr objPtr, ObjectsArr& arr);
	bool removeObject(const IDType id, ObjectsArr& arr);

	bool checkObjectsObsolete(ObjectsArr& arr);
	void setName(const std::string& name);

	float getSimulationTime() const { return _simulationTime; }
	bool getPaused() const { return _paused; }
	const Size& getSize() const;
	virtual GameObjectArrPtrVec getObjectLists();

	IDType newID() const;

	virtual bool isObjectObsolete(GameObjectPtr objPtr);

private:
	Size _size;
	bool _paused{ false };
	float _simulationTime{ 0.f };
	std::string _name;
	IDTypeFunc _newIDFunc;

	
	InvalidateObjectsFunc _invalidateObjFunc;
	//todo: can we use normal Observer here instead...
};
