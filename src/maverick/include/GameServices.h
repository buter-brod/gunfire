#pragma once
#include "MiscForward.h"
#include <functional>

typedef std::function<IDType()> IDTypeFunc;
typedef std::function<void()> InvalidateObjectsFunc;

class GameServices {

public:
	GameServices(IDTypeFunc idF, InvalidateObjectsFunc invF) noexcept;

	IDType NewID() const;
	void invalidateObjects() const;

private:
	IDTypeFunc _newIDFunc;
	InvalidateObjectsFunc _invalidateObjFunc;
};