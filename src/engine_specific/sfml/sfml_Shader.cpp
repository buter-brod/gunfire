#include "sfml_Shader.h"
#include "Log.h"

sfml_Shader::sfml_Shader(const std::string& name) : Shader(name) {
}

bool sfml_Shader::Load(const std::string& vF, const std::string& fF) {

	_vFilename = vF;
	_fFilename = fF;

	const bool vOk = _shader.loadFromFile(vF, sf::Shader::Vertex);
	if (!vOk) {
		Log::Inst()->PutErr("Shader load error " + _name + " " + vF);
		return false;
	}

	const bool fOk = _shader.loadFromFile(fF, sf::Shader::Fragment);
	if (!fOk) {
		Log::Inst()->PutErr("Shader load error " + _name + " " + fF);
		return false;
	}

	_shader.setUniform("texture", sf::Shader::CurrentTextureType());
	return true;
}

void sfml_Shader::SetUniform(const std::string& name, const float v1) {
	_shader.setUniform(name, v1);
}

void sfml_Shader::SetUniform(const std::string& name, const float v1, const float v2) {
	_shader.setUniform(name, sf::Glsl::Vec2(v1, v2));
}

void sfml_Shader::SetUniform(const std::string& name, const float v1, const float v2, const float v3, const float v4) {
	_shader.setUniform(name, sf::Glsl::Vec4(v1, v2, v3, v4));
}


ShaderPtr Shader::Create(const std::string& name) {
	return std::make_shared<sfml_Shader>(name);
}