#pragma once
#include <memory>
#include <string>

class Shader;
typedef std::shared_ptr<Shader> ShaderPtr;

class Shader {

public:

	explicit Shader(const std::string& name);
	virtual bool Load(const std::string& vF, const std::string& fF) = 0;

	virtual void SetUniform(const std::string& name, const float v1) = 0;
	virtual void SetUniform(const std::string& name, const float v1, const float v2) = 0;
	virtual void SetUniform(const std::string& name, const float v1, const float v2, const float v3, const float v4) = 0;

	static ShaderPtr Create(const std::string& name);

protected:
	std::string _name;

	std::string _vFilename;
	std::string _fFilename;
};
