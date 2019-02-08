#include "GameplayComponent.h"
#include "MiscForward.h"

class Player;
typedef std::weak_ptr<Player> PlayerWPtr;

class CFGameplayComponent : public GameplayComponent {

public:
	static std::string nameLiteral();

	explicit CFGameplayComponent(const Size& sz);

	virtual ~CFGameplayComponent();
	
	virtual void Init() override;
	virtual void OnCursorMoved(const Point& pt) override;
	virtual void OnCursorClicked(const Point& pt) override;

	virtual bool Update(const float dt) override;

	unsigned int GetFrags() const { return _frags; }
	float GetTimeRemain() const;	

protected:
	bool tryShoot(const Point& whereTo);
	void checkSpawn();

	void checkCollisions();
	void onCollision(GameObjectPtr bullet, GameObjectPtr enemy);

	bool isObjectObsolete(GameObjectPtr objPtr);

	bool isGameOverAnimStarted() const;
	void startGameOverAnim();
	void setGameOverAnimFor(GameObjectPtr objPtr);
	
	virtual void initSound();

	virtual GameObjectArrPtrVec getObjectLists() override;

private:
	GameObjectWPtr _bgWPtr;
	PlayerWPtr _playerWPtr;

	struct {
		bool requested{ false };
		Point targetPt;
	} _shootRequest;

	unsigned int _frags{ 0 };
	float _bonusTime{ 0.f };

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
	ObjectsArr _miscObjects;
};