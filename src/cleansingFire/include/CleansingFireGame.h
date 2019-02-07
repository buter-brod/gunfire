#pragma once
#include "Game.h"

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class CleansingFireGame : virtual public Game {
public:
	CleansingFireGame();
	~CleansingFireGame();

	virtual void Init() override;

	virtual void OnCursorMoved(const Point& pt) override;
	virtual void OnCursorClicked(const Point& pt) override;

protected:
	bool tryShoot(const Point& whereTo);
	void checkSpawn();

	bool isGaveOver() const;

	void checkCollisions();
	void onCollision(GameObjectPtr bullet, GameObjectPtr enemy);

	bool isObjectObsolete(GameObjectPtr objPtr) override;
	void checkAllObjectsObsolete() override;

	void onGameOver();

	virtual void initText();
	virtual void initSound();
	virtual void updateText();

	unsigned int getFrags() const { return _frags; }

	GameObjectPtr getBGObject() const { return _bgObject; }
	PlayerPtr getPlayerObject() const { return _playerObj; }

	typedef std::deque<GameObjectPtr> ObjectsArr;

	const ObjectsArr& getEnemyObjects() const  { return _enemyObjects; }
	const ObjectsArr& getBulletObjects() const { return _bulletObjects; }

	virtual bool updateSpecific(const float dt) override;
	float getTimeRemain() const;

private:
	GameObjectPtr _bgObject;
	PlayerPtr _playerObj;

	struct {
		bool requested{ false };
		Point targetPt;
	} _shootRequest;
	
		
	unsigned int _frags{ 0 };
	float _bonusTime{ 0.f };

	ObjectsArr _enemyObjects;
	ObjectsArr _bulletObjects;
};



