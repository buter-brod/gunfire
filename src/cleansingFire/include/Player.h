#pragma once
#include "GameObject.h"

class Player : public GameObject {
	
public:
	explicit Player(const IDType id);
	void Shoot();
};
