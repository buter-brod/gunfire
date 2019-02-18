#pragma once
#include "MiscForward.h"
#include "Point.h"

#include <string>
#include <map>
#include <functional>

typedef std::function<void()> InvalidateObjectsFunc;

class DialogInfo {

public:
	DialogInfo(const std::string& name, const std::string& sprite, const unsigned int count) noexcept;

	static constexpr int disabledState = -2;
	static constexpr int initialState = -1;

	bool IsActive() const;
	bool IsDisabled() const;
	bool IsFinished() const;

	void Disable();
	void Reset();
	bool Start();
	bool Advance();

	const std::string& GetSprite() const { return _sprite; }
	const std::string& GetName() const { return _name; }

	int GetState() const { return _state; }

	typedef std::shared_ptr<DialogInfo> Ptr;
	typedef std::weak_ptr<DialogInfo> WPtr;

private:

	std::string _name;
	std::string _sprite;
	int _state{ initialState };

	unsigned int _count{ 0 };
};

class DlgManager {

public:
	bool AddDialog(DialogInfo::Ptr dlgPtr);
	ObjectsArr& GetObjects();

	bool IsActive() const;

	bool StartDialog(const std::string& dlgName);
	void SetGameServicesPtr(const GameServicesWPtr& gsptr);

	bool AdvanceDialog();
	bool EndDialog();

protected:

	IDType newID() const;
	void invalidateObjects() const;

	bool tryRemove(GameObjectWPtr& wPtr);
	bool addObject(const std::string& name, const std::string& sprite, GameObjectWPtr& wptr, Size sz = Size());
	
	void refreshObjects();

	private:
	std::map<std::string, DialogInfo::Ptr> _dialogs;

	GameServicesWPtr _gameServicesWPtr;

	DialogInfo::WPtr _activeDlgPtr;

	ObjectsArr _dlgObjects;

	GameObjectWPtr _dialogBg;
	GameObjectWPtr _dialogWndBg;
	GameObjectWPtr _dialogTxt;
};
