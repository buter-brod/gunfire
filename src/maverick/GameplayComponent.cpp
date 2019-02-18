#include "GameplayComponent.h"
#include "GameObject.h"

#include "Log.h"
#include "Config.h"
#include "GameServices.h"

GameplayComponent::GameplayComponent(const Size& sz) : _size(sz) {
}

void GameplayComponent::setName(const std::string& name) {
	_name = name;
}

void GameplayComponent::Init() {}

void GameplayComponent::SetGameServicesPtr(const GameServicesWPtr& gsptr) {
	_gameServicesWPtr = gsptr;
}

const Size& GameplayComponent::getSize() const {
	return _size;
}

IDType GameplayComponent::newID() const {

	const auto gsPtr = _gameServicesWPtr.lock();

	if (gsPtr) {
		return gsPtr->NewID();
	}

	Log::Inst()->PutErr("GameplayComponent::newID error, id func not found");
	return 0;
}

void GameplayComponent::invalidateObjects() const {
	
	const auto gsPtr = _gameServicesWPtr.lock();
	if (gsPtr) {
		gsPtr->invalidateObjects();
	}
}

bool GameplayComponent::addObject(GameObjectPtr objPtr, ObjectsArr& arr) const {

	const IDType id = objPtr->getId();

	if (arr.count(id) > 0) {
		Log::Inst()->PutErr("GameplayComponent::addObject error, object " + objPtr->getFullName() + " already added");
		return false;
	}
	
	arr.emplace(id, objPtr);
	invalidateObjects();

	return true;
}

bool GameplayComponent::removeObject(const IDType id, ObjectsArr& arr) const {
	if (arr.count(id) == 0) {
		Log::Inst()->PutErr("GameplayComponent::removeObject error, object " + std::to_string(id) + " already removed or never existed");
		return false;
	}

	arr.erase(id);
	invalidateObjects();

	return true;
}

bool GameplayComponent::removeObject(GameObjectPtr objPtr, ObjectsArr& arr) const {
	
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

void GameplayComponent::schedule(std::function<void()> f) {

	if(!f) {
		Log::Inst()->PutErr("GameplayComponent::schedule error, f null");
	}

	_scheduled.push(f);
}

bool GameplayComponent::Update(const float dt) {

	while (!_scheduled.empty()) {
		const auto f = _scheduled.front();
		f();
		_scheduled.pop();
	}

	const float t = _paused ? 0.f : dt;
	_simulationTime += t;
	const auto& arrs = getObjectLists();

	for (auto& arr : arrs) {
		
		for (auto& objPair : *arr) {
			auto objPtr = objPair.second;
			objPtr->Update(t, _simulationTime);
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

