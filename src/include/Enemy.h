#pragma once

#include "GameObject.h"
#include <memory>

class Enemy : public GameObject {

public:
	Enemy(const IDType id);

	void Boom(const Point& bulletPos);
	
	void Update(float dt) override;
	ShaderPtr GetShader() override;

protected:
	virtual Point getEmitterPosition();

private:
	Point _boomPosition;
};

typedef std::shared_ptr<Enemy> EnemyPtr;