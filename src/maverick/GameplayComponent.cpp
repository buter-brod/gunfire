#include "GameplayComponent.h"
#include "GameObject.h"

#include "Log.h"
#include "Config.h"

GameplayComponent::GameplayComponent(const Size& sz) : _size(sz) {
}

void GameplayComponent::setName(const std::string& name) {
	_name = name;
}

void GameplayComponent::Init() {}

void GameplayComponent::SetObjInvalidateFunc(const InvalidateObjectsFunc f) {
	if (_invalidateObjFunc) {
		Log::Inst()->PutErr("GameplayComponent::SetObjInvalidateFunc error, you're not trying to add one component to more than one game, are you?");
	}
	_invalidateObjFunc = f;
}

void GameplayComponent::SetIDFunc(const IDTypeFunc f) {

	if (_newIDFunc) {
		Log::Inst()->PutErr("GameplayComponent::SetIDFunc error, you're not trying to add one component to more than one game, are you?");
	}
	_newIDFunc = f;
}

const Size& GameplayComponent::getSize() const {
	return _size;
}

IDType GameplayComponent::newID() const {

	if (_newIDFunc) {
		return _newIDFunc();
	}

	Log::Inst()->PutErr("GameplayComponent::newID error, id func not found");
	return 0;
}

bool GameplayComponent::addObject(GameObjectPtr objPtr, ObjectsArr& arr) {

	const IDType id = objPtr->getId();

	if (arr.count(id) > 0) {
		Log::Inst()->PutErr("GameplayComponent::addObject error, object " + objPtr->getFullName() + " already added");
		return false;
	}
	
	arr.emplace(id, objPtr);

	if (_invalidateObjFunc) {
		_invalidateObjFunc();
	}

	return true;
}

bool GameplayComponent::removeObject(const IDType id, ObjectsArr& arr) {
	if (arr.count(id) == 0) {
		Log::Inst()->PutErr("GameplayComponent::removeObject error, object " + std::to_string(id) + " already removed or never existed");
		return false;
	}

	arr.erase(id);

	if (_invalidateObjFunc) {
		_invalidateObjFunc();
	}

	return true;
}

bool GameplayComponent::removeObject(GameObjectPtr objPtr, ObjectsArr& arr) {
	
	const IDType id = objPtr->getId();
	return removeObject(id, arr);
}

void GameplayComponent::OnCursorMoved(const Point& pt) {}
void GameplayComponent::OnCursorClicked(const Point& pt) {}

bool GameplayComponent::isObjectObsolete(GameObjectPtr objPtr) {
	return false;
}

void GameplayComponent::SetPaused(const bool paused) {
	_paused = paused;
}

bool GameplayComponent::checkObjectsObsolete(ObjectsArr& arr) {

	std::set<IDType> obsoletes;

	for (const auto& obj : arr) {
		if (isObjectObsolete(obj.second)) {
			obsoletes.insert(obj.second->getId());
		}
	}

	if (obsoletes.empty()) {
		return false;
	}

	for (const IDType obsId : obsoletes) {
		removeObject(obsId, arr);
	}

	return true;
}

bool GameplayComponent::Update(const float dt) {

	if (_paused) {
		return false;
	}

	_simulationTime += dt;

	const auto& arrs = getObjectLists();

	for (auto& arr : arrs) {
		
		for (auto& objPair : *arr) {
			auto objPtr = objPair.second;
			objPtr->Update(dt, _simulationTime);
		}

		checkObjectsObsolete(*arr);
	}

	return true;
}

GameObjectCArrPtrVec GameplayComponent::GetObjectLists() {
	
	const auto& lists = getObjectLists();
	return { lists.begin(), lists.end() };
}

GameObjectArrPtrVec GameplayComponent::getObjectLists() {
	return {};
}

