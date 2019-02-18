#include "Dialog.h"
#include "Log.h"
#include "GameObject.h"
#include "GameServices.h"
#include "CFCfgStatic.h"

DialogInfo::DialogInfo(const std::string& name, const std::string& sprite, const unsigned int count) noexcept
: _name(name), _sprite(sprite), _count(count){

	if (name.empty() || sprite.empty() || count < 1) {
		Log::Inst()->PutErr("DialogInfo error, trying to create invalid dlg object, name " + name + " sprite " + sprite + " count " + std::to_string(count));
	}
}

bool DialogInfo::Start() {
	if (_state != initialState || _count < 1) {
		Log::Inst()->PutErr("DialogInfo::Start error, name " + _name + ", sprite " + _sprite + ", state " + std::to_string(_state));
		return false;
	}

	_state = 0;
	return true;
}

bool DialogInfo::IsFinished() const {
	return _state == (int)_count - 1;
}

bool DialogInfo::Advance() {

	const bool disabled = IsDisabled();
	const bool finished = IsFinished();

	if (disabled) {
		Log::Inst()->PutErr("DialogInfo::Advance error, name " + _name + ", sprite " + _sprite + ", state " + std::to_string(_state));
		return false;
	}

	if (finished) {
		Log::Inst()->PutMessage("DialogInfo::Advance: will not advance further. name " + _name + ", state " + std::to_string(_state));
		return false;
	}

	++_state;
	return true;
}

bool DialogInfo::IsActive() const {
	return _state >= 0;
}

bool DialogInfo::IsDisabled() const {
	return (_state == disabledState);
}

void DialogInfo::Disable() {
	_state = disabledState;
}

void DialogInfo::Reset() {
	_state = initialState;
}

ObjectsArr& DlgManager::GetObjects() {
	return _dlgObjects;
}

bool DlgManager::AddDialog(DialogInfo::Ptr dlgPtr) {
	
	if (!dlgPtr) {
		Log::Inst()->PutErr("DlgManager::AddDialog error, dlg not specified");
		return false;
	}

	const std::string& name = dlgPtr->GetName();

	if (_dialogs.find(name) != _dialogs.end()) {
		Log::Inst()->PutErr("DlgManager::AddDialog error, name already used " + name);
		return false;
	}

	_dialogs.emplace(name, dlgPtr);

	Log::Inst()->PutMessage("DlgManager::AddDialog " + dlgPtr->GetName());

	return true;
}

bool DlgManager::StartDialog(const std::string& dlgName) {

	const auto dlgIt = _dialogs.find(dlgName);
	if (dlgIt == _dialogs.end()) {
		Log::Inst()->PutErr("DlgManager::StartDialog error, name not found " + dlgName);
		return false;
	}

	auto& dlgObjPtr = dlgIt->second;

	dlgObjPtr->Reset();
	dlgObjPtr->Start();

	_activeDlgPtr = dlgObjPtr;

	Log::Inst()->PutMessage("DlgManager::AdvanceDialog " + dlgObjPtr->GetName());

	refreshObjects();

	return true;
}

bool DlgManager::AdvanceDialog() {
	
	const auto activeDlgObj = _activeDlgPtr.lock();

	if (!activeDlgObj) {
		Log::Inst()->PutErr("DlgManager::AdvanceDialog error, no active dialog");
		return false;
	}

	const bool advanced = activeDlgObj->Advance();

	if (!advanced) {
		return false;
	}

	Log::Inst()->PutMessage("DlgManager::AdvanceDialog " + activeDlgObj->GetName());

	refreshObjects();
	return true;
}

bool DlgManager::EndDialog() {
	
	const auto activeDlgObj = _activeDlgPtr.lock();

	if (!activeDlgObj) {
		Log::Inst()->PutErr("DlgManager::EndDialo error, no active dialog");
		return false;
	}

	activeDlgObj->Reset();
	_activeDlgPtr.reset();

	Log::Inst()->PutMessage("DlgManager::EndDialog " + activeDlgObj->GetName());

	refreshObjects();

	return true;
}

bool DlgManager::addObject(const std::string& name, const std::string& sprite, GameObjectWPtr& wptr, Size sz){

	Log::Inst()->PutMessage("DlgManager::addObject " + name + ", " + sprite);

	const auto obj = std::make_shared<GameObject>(newID(), name, sprite);
	
	if (!sz.isEmpty()) {
		obj->SetSize(sz);
	}

	const Point center = CfgStatic::gameSize / 2.f;
	obj->SetPosition(center);

	const IDType id = obj->getId();

	if (_dlgObjects.count(id) > 0) {
		Log::Inst()->PutErr("DlgManager::addObject error, object " + obj->getFullName() + " already added");
		return false;
	}

	_dlgObjects.emplace(id, obj);
	invalidateObjects();

	wptr = obj;

	return true;
}

bool DlgManager::IsActive() const {
	return _activeDlgPtr.lock() != nullptr;
}

bool DlgManager::tryRemove(GameObjectWPtr& wPtr) {
	const auto obj = wPtr.lock();
	if (obj) {
		_dlgObjects.erase(obj->getId());
		wPtr.reset();

		Log::Inst()->PutMessage("DlgManager removed object " + obj->getFullName());

		invalidateObjects();
		return true;
	}
	return false;
}

void DlgManager::refreshObjects() {

	const auto activeDlgPtr = _activeDlgPtr.lock();

	tryRemove(_dialogTxt);

	if (!activeDlgPtr) {
		tryRemove(_dialogBg);
		tryRemove(_dialogWndBg);

	} else {

		const auto createFunc = [this](const std::string& name, const std::string& sprite, GameObjectWPtr& wptr) {
			if (!wptr.lock()) {
				addObject(name, sprite, wptr);
			}
		};

		createFunc(CfgStatic::bgDlgName, CfgStatic::bgDlgSpr, _dialogBg);
		createFunc(CfgStatic::bgWndDlgName, CfgStatic::bgWndDlgSpr, _dialogWndBg);

		std::string introDlgSprName = activeDlgPtr->GetSprite();
		const int state = activeDlgPtr->GetState();

		const auto dotPos = introDlgSprName.find('.'); // if names contain file extension, put frame number at the end of the name-part.
		introDlgSprName.insert(dotPos, std::to_string(state));

		createFunc(CfgStatic::textDlgName, introDlgSprName, _dialogTxt);
	}	
}

IDType DlgManager::newID() const {
	
	const auto gsPtr = _gameServicesWPtr.lock();

	if (gsPtr) {
		return gsPtr->NewID();
	}

	Log::Inst()->PutErr("GameplayComponent::newID error, id func not found");
	return 0;
}

void DlgManager::invalidateObjects() const {
	
	const auto gsPtr = _gameServicesWPtr.lock();
	if (gsPtr) {
		gsPtr->invalidateObjects();
	}
}

void DlgManager::SetGameServicesPtr(const GameServicesWPtr& gsptr) {
	_gameServicesWPtr = gsptr;
}
