#pragma once
#include "GameObject.h"
#include <memory>

class Player : public GameObject {
	
public:
	explicit Player(const IDType id);
	void Shoot();
};

typedef std::shared_ptr<Player> PlayerPtr;