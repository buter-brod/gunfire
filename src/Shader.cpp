#include "Shader.h"
#include "Log.h"

Shader::Shader(const std::string& name) : _name(name) {	
}

bool Shader::Load(const std::string& vF, const std::string& fF) {

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