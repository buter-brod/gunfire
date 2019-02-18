#include "GameServices.h"
#include "Log.h"

GameServices::GameServices(IDTypeFunc idF, InvalidateObjectsFunc invF) noexcept 
: _newIDFunc(idF), _invalidateObjFunc(invF) {
}

IDType GameServices::NewID() const {

	if (!_newIDFunc) {
		Log::Inst()->PutErr("GameServices::NewID error, no func");
		return 0;
	}

	return _newIDFunc();
}

void GameServices::invalidateObjects() const {

	if (!_invalidateObjFunc) {
		Log::Inst()->PutMessage("GameServices::invalidateObjects warning, no func provided.. ");
		return;
	}

	return _invalidateObjFunc();
}