#include "Game.h"

#include "Config.h"
#include "Log.h"
#include "SoundManager.h"
#include "GameObject.h"

#include <algorithm>

IDType Game::newID() {
	return _nextID++;
}

Game::Game() {

	Config::Inst();
	Log::Inst()->PutMessage("Game::Game");
}

Game::~Game() {

	Log::Inst()->PutMessage("Game::~Game");
}

Size Game::GetSize() const {
	return CfgStatic::gameSize;
}

void Game::checkAllObjectsObsolete() {}

void Game::updateAllObjects(const float dt) {

	for (auto& objW : _allObjects) {
		auto obj = objW.second.lock();

		if (!obj) {
			Log::Inst()->PutErr("Game::checkObjectsObsolete error, " + obj->getFullName() + " not found?! ");
			continue;
		}

		obj->Update(dt, _simulationTime);
	}
}

bool Game::updateSpecific(const float dt) {
	return true;
}

bool Game::update(const float dt) {
	if (_paused) {
		return false;
	}

	_simulationTime += dt;

	const bool updated = updateSpecific(dt);

	checkAllObjectsObsolete();
	updateAllObjects(dt);
	
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

bool Game::isObjectObsolete(GameObjectPtr objPtr) {

	if (objPtr->GetState() == CfgStatic::deadStateName) {
		return true;
	}

	return false;
}

bool Game::checkObjectsObsolete(ObjectsArr& arr) {

	std::set<IDType> obsoletes;

	for (const auto& obj : arr) {
		if (isObjectObsolete(obj)) {
			obsoletes.insert(obj->getId());
		}
	}

	if (obsoletes.empty()) {
		return false;
	}

	for (const IDType obsId : obsoletes) {
		const auto objIt = std::find_if(arr.begin(), arr.end(), [obsId](const ObjectsArr::value_type& obj) { return obj->getId() == obsId; });

		if (objIt != arr.end()) {
			removeObject(*objIt, arr);
		}
		else {
			Log::Inst()->PutErr("Game::checkObjectsObsolete error, id " + std::to_string(obsId) + " not found?! ");
		}
	}

	return true;
}

GameObjectPtr Game::GetObject(const IDType id) const {

	const auto& objWPtrIt = _allObjects.find(id);

	if (objWPtrIt != _allObjects.end()) {

		auto existObjectPtr = objWPtrIt->second.lock();
		if (existObjectPtr) { // object is alive
			return existObjectPtr;
		}
		else {
			Log::Inst()->PutErr("Game::GetObject error, id " + std::to_string(id) + " found in registry but object does not exist!");
		}
	}
	else {
		// that's ok, object with that id may not exist in game
	}

	return GameObjectPtr();
}

bool Game::addObject(GameObjectPtr objPtr) {

	const IDType id = objPtr->getId();

	if (_allObjects.count(id) == 0) {
		_allObjects[id] = objPtr;
	}
	else {
		Log::Inst()->PutErr("Game::addObject error, _allObjects already has id " + std::to_string(id));
		return false;
	}

	return true;
}

bool Game::addObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);

	if (objIt != arr.end()) {
		return false;
	}

	arr.push_back(objPtr);
	const bool added = addObject(objPtr);
	return added;
}

bool Game::removeObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const auto& objIt = std::find(arr.begin(), arr.end(), objPtr);

	if (objIt == arr.end()) {
		return false;
	}

	const IDType id = objPtr->getId();

	if (_allObjects.count(id) == 1) {
		_allObjects.erase(id);
	}
	else {
		Log::Inst()->PutErr("Game::removeObject error, _allObjects hasn't id " + std::to_string(id));
	}

	arr.erase(objIt);
	return true;
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
