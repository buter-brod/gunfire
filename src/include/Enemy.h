#pragma once
#include "GameObject.h"
#include <memory>

class Enemy : public GameObject {

public:
	Enemy(const IDType id);

	void Boom(const Point& bulletPos);
	
	void Update(const float dt, const float gameTime) override;
	ShaderPtr GetShader() override;
	float GetPixelizeCoeff() const;

protected:
	virtual Point GetEmitterPosition() const override;

private:
	Point _boomPosition;
};

typedef std::shared_ptr<Enemy> EnemyPtr;