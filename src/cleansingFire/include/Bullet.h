#pragma once

#include "GameObject.h"
#include <memory>

class Bullet : public GameObject {

public:

	explicit Bullet(const IDType id);
	
	void Boom();
	virtual bool RequestKill(const std::string& reason) override;

protected:
	Point GetEmitterPosition() const override;
};

typedef std::shared_ptr<Bullet> BulletPtr;