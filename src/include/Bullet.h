#pragma once

#include "GameObject.h"
#include <memory>

class Bullet : public GameObject {

public:

	Bullet(const IDType id);
	void Boom();

protected:
	Point GetEmitterPosition() const override;
};

typedef std::shared_ptr<Bullet> BulletPtr;