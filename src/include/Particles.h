#pragma once
#include <memory>
#include <string>
#include <set>
#include "Thor/Particles/ParticleSystem.hpp"
#include "Thor/Particles/Emitters.hpp"

class Particles;
class Emitter;

typedef std::shared_ptr<Emitter> EmitterPtr;
typedef std::shared_ptr<Particles> ParticlesPtr;

namespace Utils {
	class Point;
}

class Emitter {

public:
	Emitter() noexcept {}
	~Emitter() {}
	thor::UniversalEmitter* get() { return &_e; }

	void SetPosDeviation(const float d) { _posDeviation = d; }
	void SetPosition(const Utils::Point& p);
	void Stop();

protected:
	thor::UniversalEmitter _e;
	float _posDeviation{ 0.f };
};

class Particles : public thor::ParticleSystem {

public:
	void StopEmitters();
	bool AddEmitter(EmitterPtr emitterPtr);
	void Update(const float dt, const Utils::Point& p);

	ParticlesPtr static Build(const std::string& config);

private:
	std::set<EmitterPtr> _emitters;
};
