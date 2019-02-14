#pragma once

#ifndef NO_THOR

#include "Point.h"

#include "Thor/Particles/ParticleSystem.hpp"
#include "Thor/Particles/Emitters.hpp"

#include <memory>
#include <set>

class Particles;
class Emitter;

typedef std::shared_ptr<Emitter> EmitterPtr;
typedef std::shared_ptr<Particles> ParticlesPtr;

class Emitter {

public:
	Emitter() noexcept {}
	~Emitter() {}
	thor::UniversalEmitter* get() { return &_e; }

	void SetPosDeviation(const float d) { _posDeviation = d; }
	void SetPosition(const Point& p);
	void Stop();

protected:
	thor::UniversalEmitter _e;
	float _posDeviation{ 0.f };
};

class Particles : public thor::ParticleSystem {

public:
	void StopEmitters();
	bool AddEmitter(EmitterPtr emitterPtr);

	void Update(const float dt, const Point& p);

	ParticlesPtr static Build(const std::string& config);

private:
	std::set<EmitterPtr> _emitters;
};

#endif