#include "GameplayComponent.h"
#include "MiscForward.h"

class Player;
typedef std::weak_ptr<Player> PlayerWPtr;

class CFGameplayComponent : public GameplayComponent {

public:
	static const std::string& nameLiteral();

	explicit CFGameplayComponent(const Size& sz);

	virtual ~CFGameplayComponent();
	
	virtual void Init() override;
	virtual void OnCursorMoved(const Point& pt) override;
	virtual void OnCursorClicked(const Point& pt) override;

	virtual bool Update(const float dt) override;

	bool IsIntroDialogActive() const;
	bool IsOutroDialogActive() const;
	void SetSkipIntro();

	unsigned int GetFrags() const { return _frags; }
	float GetTimeRemain() const;

protected:
	bool tryShoot(const Point& whereTo);
	void checkSpawn();

	void checkCollisions();
	void onCollision(GameObjectPtr bullet, GameObjectPtr enemy);

	bool isObjectObsolete(GameObjectPtr objPtr) override;

	bool isGameOverAnimStarted() const;
	void startGameOverAnim();
	void setGameOverAnimFor(const GameObjectPtr& obj) const;

	bool checkIntroDlg();
	bool checkOutroDlg();
	bool removeDlg();
	bool addDlg(const std::string& sprName, const unsigned int number);
	void tryDlgAdvance();
	
	virtual void initSound();

	float getTimeRemain() const;

	virtual GameObjectArrPtrVec getObjectLists() override;

private:
	GameObjectWPtr _bgWPtr;
	PlayerWPtr _playerWPtr;
	mutable float _timeRemain { -1.f };

	struct {
		bool requested{ false };
		Point targetPt;
	} _shootRequest;

	unsigned int _frags{ 0 };
	float _bonusTime{ 0.f };

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _miscObjects;
	ObjectsArr _dlgObjects;

	GameObjectWPtr _dialogBg;
	GameObjectWPtr _dialogWndBg;
	GameObjectWPtr _dialogTxt;

	std::string _musicTrack;
	std::string _ambientTrack;

	int _introDlgState{ 0 };
	int _outroDlgState{ 0 };
};