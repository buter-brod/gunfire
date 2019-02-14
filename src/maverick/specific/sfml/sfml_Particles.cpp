#include "sfml_Particles.h"

#ifndef NO_THOR

#include "Log.h"
#include "Thor/Math/Distributions.hpp"

void Particles::StopEmitters() {

	for (const auto& e : _emitters) {
		e->Stop();
	}
}

void Particles::Update(const float dt, const Point& p) {
	
	for (const auto& e : _emitters) {
		e->SetPosition(p);
	}

	update(sf::seconds(dt));
}

bool Particles::AddEmitter(EmitterPtr emitterPtr) {

	if (_emitters.count(emitterPtr) > 0) {
		Log::Inst()->PutErr("sfml_Particles::addEmitter error, emitter already added");
		return false;
	}

	_emitters.insert(emitterPtr);
	addEmitter(thor::refEmitter(*emitterPtr->get()));
	return true;
}

void Emitter::SetPosition(const Point& p) {
	get()->setParticlePosition(thor::Distributions::circle({ p.getX(), p.getY() }, _posDeviation));
}

void Emitter::Stop() {
	get()->setEmissionRate(0.f);
}

#endif