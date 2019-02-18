#include "GameplayComponent.h"
#include "MiscForward.h"

class CFGameplayComponent : public GameplayComponent {

public:
	static const std::string& nameLiteral();

	explicit CFGameplayComponent(const Size& sz);

	virtual ~CFGameplayComponent();
	
	virtual void Init() override;
	virtual void OnCursorMoved(const Point& pt) override;
	virtual void OnCursorClicked(const Point& pt) override;

	virtual bool Update(const float dt) override;

	void SetSkipIntro();

	unsigned int GetFrags() const { return _frags; }
	float GetTimeRemain() const;

	enum class GameState {
		INIT,
		INTRO,
		MAIN,
		GAMEOVER,
		OUTRO
	};

	GameState GetState() const { return _state; }

protected:
	bool tryShoot(const Point& whereTo);
	void checkSpawn();

	void checkCollisions();
	void onCollision(const GameObjectPtr& bullet, const GameObjectPtr& enemy);

	bool isObjectObsolete(GameObjectPtr objPtr) override;

	void startGameOverAnim();
	void setGameOverAnimFor(const GameObjectPtr& obj) const;

	static bool initCfgSound(const std::string& cfgTrack, std::string& outputStr);

	virtual void initSound();
	virtual void initDialogs();

	float getTimeRemain() const;

	virtual GameObjectArrPtrVec getObjectLists() override;

private:
	GameObjectWPtr _bgWPtr;
	PlayerWPtr _playerWPtr;
	mutable float _timeRemain { -1.f };

	unsigned int _frags{ 0 };
	float _bonusTime{ 0.f };

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _miscObjects;

	std::string _musicTrack;
	std::string _ambientTrack;

	DlgManagerPtr _dlgMgr;

	bool _skipIntro{ false };

	GameState _state { GameState::INIT };

	bool tryStateAdvance();
	void setState(const GameState state);
};