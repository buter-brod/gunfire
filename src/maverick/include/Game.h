#pragma once
#include "MiscForward.h"
#include "Point.h"

typedef std::map<std::string, GameObjectCArrPtrVec> ObjListsPerComponentMap;

class Game {
public:
	Game();
	virtual ~Game();

	virtual void Init();
	virtual void Draw() = 0;
	virtual bool Update(const float dt);
	IDType NewID();

	virtual void OnCursorMoved(const Point& pt);
	virtual void OnCursorClicked(const Point& pt);

	bool GetPaused() const;
	bool SetPaused(const bool paused);

	float GetSimulationTime() const { return _simulationTime; };

protected:
	const Size& getSize() const;

	virtual bool update(const float dt);
	virtual bool updateGameplay(const float dt);

	const std::vector<GameObjectWPtr>& getObjects() const;
	ObjListsPerComponentMap getObjectLists() const;

	template <class CompType> std::shared_ptr<CompType> addGameplayComponent() {
		auto compPtr = std::make_shared<CompType>(getSize());
		addGameplayComponent(compPtr);
		compPtr->Init();
		return compPtr;
	}

	bool addGameplayComponent(GameplayComponentPtr component);

	GameplayComponentPtr getGameplayComponent(const std::string& name) const;

	void invalidateCache() const;

	float getSimulationTime() const { return _simulationTime; }

private:
	bool _paused{ false };
	
	float _simulationTime{ 0.f };
	float _simulationTimeAcc{ 0.f };

	mutable std::vector<GameObjectWPtr> _objectsCache;

	std::map<std::string, GameplayComponentPtr> _gameplayComponents;

	SoundManagerWPtr _soundMgr;
	IDType _nextID{ 1 };

	friend class GameplayComponent;
};



