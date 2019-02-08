#include "ShaderHelper.h"
#include "Config.h"
#include "GameObject.h"
#include "Shader.h"
#include "Log.h"
#include "EngineComponent.h"

bool ConfigureShader(const std::string& shaderName, const GameObject* obj) {

	if (shaderName == CfgStatic::pixelizeShader) {

		auto engineComp = obj->GetEngineComponent();
		ShaderPtr shader = engineComp->GetShader();
		
		if (!engineComp) {
			Log::Inst()->PutErr("ConfigureShader error for " + obj->getFullName() + ", engineComp not found");
			return false;
		}

		if (!shader) {
			Log::Inst()->PutErr("ConfigureShader error for " + obj->getFullName() + ", shader not found");
			return false;
		}

		const Size& bigSize = engineComp->GetTextureSize();
		const Rect& smallRect = engineComp->GetSpriteRect();

		const float texRect[4] = {
				smallRect._origin.getX(),
				bigSize.getY() - smallRect._origin.getY(),
				smallRect._size.getX(),
				smallRect._size.getY() };

		shader->SetUniform("bigRectSize", bigSize.getX(), bigSize.getY());
		shader->SetUniform("smallRect", texRect[0], texRect[1], texRect[2], texRect[3]);

		const float pixelizeCoeffMin = 1.f;

		const float coeff = obj->GetEffectCoeff(CfgStatic::pixelizeDuration, pixelizeCoeffMin, CfgStatic::pixelizeCoeffMax);

		shader->SetUniform("coeff", coeff);

		return true;
	}

	return false;
}