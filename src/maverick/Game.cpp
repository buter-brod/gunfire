#include "Game.h"

#include "GameplayComponent.h"
#include "Config.h"
#include "CfgStatic.h"
#include "Log.h"

IDType Game::NewID() {
	return _nextID++;
}

Game::Game() {
	Config::Inst();
	Log::Inst()->PutMessage("Game::Game");
}

Game::~Game() {
	Log::Inst()->PutMessage("Game::~Game");
}

const Size& Game::getSize() const {
	return CfgStatic::gameSize;
}

void Game::OnCursorMoved(const Point& pt) {

	for (auto& cmpPair : _gameplayComponents) {
		cmpPair.second->OnCursorMoved(pt);
	}
}
void Game::OnCursorClicked(const Point& pt) {

	for (auto& cmpPair : _gameplayComponents) {
		cmpPair.second->OnCursorClicked(pt);
	}
}

bool Game::addGameplayComponent(GameplayComponentPtr component) {

	const std::string& name = component->GetName();

	if (_gameplayComponents.count(name) > 0) {
		Log::Inst()->PutErr("Game::addGameplayComponent error, already exists: " + name);
		return false;
	}

	component->SetObjInvalidateFunc(std::bind(&Game::invalidateCache, this));
	component->SetIDFunc(std::bind(&Game::NewID, this));

	_gameplayComponents.emplace(name, component);
	return true;
}

void Game::invalidateCache() const {
	_objectsCache.clear();
}

GameplayComponentPtr Game::getGameplayComponent(const std::string& name) const {
	
	GameplayComponentPtr ptr;

	const auto& cmpIt = _gameplayComponents.find(name);
	if (cmpIt != _gameplayComponents.end()) {
		ptr = cmpIt->second;
	}
	else {
		Log::Inst()->PutErr("Game::getGameplayComponent error, " + name + " not found ");
	}

	return ptr;
}

bool Game::updateGameplay(const float dt) {

	for (auto& cmpPair : _gameplayComponents) {
		cmpPair.second->Update(dt);
	}
	return true;
}

bool Game::update(const float dt) {
	if (_paused) {
		return false;
	}

	_simulationTime += dt;
	updateGameplay(dt);
	return true;
}

bool Game::Update(const float frameDt) {

	bool updated = false;
	const float dt = 1.f / CfgStatic::simulationFPS;
	_simulationTimeAcc += frameDt;

	while (_simulationTimeAcc >= dt) {
		update(dt);
		_simulationTimeAcc -= dt;
		updated |= true;
	}

	return updated;
}

const std::vector<GameObjectWPtr>& Game::getObjects() const {

	if (!_objectsCache.empty()) {
		// no objects added/deleted since last time, returning cached values
		return _objectsCache;
	}

	const auto& listPerComponentMap = getObjectLists();

	for (const auto& compList : listPerComponentMap) {
		const auto& objLists = compList.second;

		for (const auto objList : objLists) {

			for (const auto& objPair : *objList) {
				const auto obj = objPair.second;
				_objectsCache.push_back(obj);
			}
		}
	}

	return _objectsCache;
}

ObjListsPerComponentMap Game::getObjectLists() const {
	ObjListsPerComponentMap lists;

	for (const auto& componentPair : _gameplayComponents) {
		const auto component = componentPair.second;
		const std::string& name = componentPair.first;
		const auto& compLists = component->GetObjectLists();
		lists.emplace(name, compLists);
	}

	return lists;
}

void Game::Init() {
	update(0.f);
}

bool Game::GetPaused() const {
	return _paused;
}

bool Game::SetPaused(const bool paused) {

	if (_paused == paused) {
		Log::Inst()->PutMessage(Log::Channel::VERBOSE, "Game::SetPaused, _paused already " + std::string(paused ? "1" : "0"));
		return false;
	}

	_paused = paused;
	return true;
}
