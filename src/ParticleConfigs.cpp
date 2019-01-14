#include "Particles.h"

#include "ResourceManager.h"
#include "Config.h"

#include "Thor/Particles/Affectors.hpp"
#include "Thor/Math/Distributions.hpp"
#include "Thor/Animations/FadeAnimation.hpp"

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
		ePtr->setParticleColor(CfgStatic::smokeClr);

		ePtr->setParticleVelocity(thor::Distributions::deflect({ 0.f, -CfgStatic::smokeSpeed }, CfgStatic::smokeMaxDeflect));
		ePtr->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));

		ePtr->setEmissionRate(CfgStatic::smokeEmission);
		ePtr->setParticleLifetime(sf::seconds(CfgStatic::smokeLifetime));

		particlesPtr->AddEmitter(emitter);

		thor::FadeAnimation fader(CfgStatic::smokeFadeIn, CfgStatic::smokeFadeOut);
		particlesPtr->addAffector(thor::AnimationAffector(fader));
	}

	else if (config == "boom") {

		const auto& texRect = ResourceManager::Inst()->GetTexture(CfgStatic::smokePng);

		if (texRect.isEmpty()) {
			return particlesPtr;
		}

		particlesPtr = std::make_shared<ParticlesPtr::element_type>();

		particlesPtr->setTexture(*texRect.texturePtr.lock()->getTex());
		unsigned int index0 = particlesPtr->addTextureRect(texRect.rect);
		
		sf::Color fireColor1(128, 25, 0);
		sf::Color fireColor2(255, 255, 165);

		EmitterPtr emitter1 = std::make_shared<EmitterPtr::element_type>();
		EmitterPtr emitter2 = std::make_shared<EmitterPtr::element_type>();

		auto e1Ptr = emitter1->get();
		auto e2Ptr = emitter2->get();
		
		e1Ptr->setParticleTextureIndex(index0);
		e2Ptr->setParticleTextureIndex(index0);
		
		e1Ptr->setParticleColor(fireColor1);
		e2Ptr->setParticleColor(fireColor2);

		e1Ptr->setEmissionRate(200);
		e2Ptr->setEmissionRate(200);

		e1Ptr->setParticleVelocity(thor::Distributions::deflect({ 0.f, 200.f }, 45.f));
		e2Ptr->setParticleVelocity(thor::Distributions::deflect({ 0.f, 150.f }, 45.f));

		e1Ptr->setParticleLifetime(sf::seconds(0.5f));
		e2Ptr->setParticleLifetime(sf::seconds(0.25f));
		
		e1Ptr->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));
		e2Ptr->setParticleRotation(thor::Distributions::uniform(0.f, 360.f));

		thor::FadeAnimation fader(0.05f, 0.55f);
		particlesPtr->addAffector(thor::AnimationAffector(fader));
		
		particlesPtr->AddEmitter(emitter1);
		particlesPtr->AddEmitter(emitter2);
	}

	return particlesPtr;
}