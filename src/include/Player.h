#pragma once
#include "GameObject.h"
#include <memory>

class Player : public GameObject {
	
public:
	Player(const IDType id, const GameWPtr game);
	void Shoot();
};

typedef std::shared_ptr<Player> PlayerPtr;