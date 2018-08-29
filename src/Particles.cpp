#include "Particles.h"

#include "ResourceManager.h"
#include "Config.h"

#include "Thor/Particles/Affectors.hpp"
#include "Thor/Math/Distributions.hpp"
#include "Thor/Animations/FadeAnimation.hpp"

void Particles::Update(const float dt, const Utils::Point& p) {
	
	for (const auto& e : _emitters) {
		e->SetPosition(p);
	}

	update(sf::seconds(dt));
}

bool Particles::AddEmitter(EmitterPtr emitterPtr) {

	if (_emitters.count(emitterPtr) > 0) {
		Log::Inst()->PutErr("Particles::addEmitter error, emitter already added");
		return false;
	}

	_emitters.insert(emitterPtr);
	addEmitter(thor::refEmitter(*emitterPtr->get()));
	return true;
}

void Emitter::SetPosition(const Point& p) {
	get()->setParticlePosition(thor::Distributions::circle({ p.getX(), p.getY() }, _posDeviation));
}

ParticlesPtr Particles::Build(const std::string& config) {

	ParticlesPtr particlesPtr;

	if (config == "smoke") {

		const auto& texRect = ResourceManager::Inst()->GetTexture(CfgStatic::smokePng);

		if (texRect.isEmpty()) {
			return particlesPtr;
		}

		particlesPtr = std::make_shared<ParticlesPtr::element_type>();

		particlesPtr->setTexture(*texRect.texturePtr.lock()->getTex());
		unsigned int index0 = particlesPtr->addTextureRect(texRect.rect);

		EmitterPtr emitter = std::make_shared<EmitterPtr::element_type>();

		emitter->SetPosDeviation(15.f);

		auto ePtr = emitter->get();

		ePtr->setParticleTextureIndex(index0);
		ePtr->setParticleColor(CfgStatic::smokeColor);

		ePtr->setParticleVelocity(thor::Distributions::deflect({ 0.f, -CfgStatic::smokeSpeed }, CfgStatic::smokeMaxDeflect));
		ePtr->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));
	
		ePtr->setEmissionRate(CfgStatic::smokeEmission);
		ePtr->setParticleLifetime(sf::seconds(CfgStatic::smokeLifetime));

		particlesPtr->AddEmitter(emitter);

		thor::FadeAnimation fader(CfgStatic::smokeFadeIn, CfgStatic::smokeFadeOut);
		particlesPtr->addAffector(thor::AnimationAffector(fader));
	}

	return particlesPtr;
}