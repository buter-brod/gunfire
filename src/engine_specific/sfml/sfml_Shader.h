#pragma once
#include <memory>

#include "Shader.h"
#include <SFML/Graphics/Shader.hpp>

class sfml_Shader : public Shader {

public:

	sfml_Shader(const std::string& name);

	sf::Shader* get() { return &_shader; }
	bool Load(const std::string& vF, const std::string& fF) override;

	void SetUniform(const std::string& name, const float v1) override;
	void SetUniform(const std::string& name, const float v1, const float v2) override;
	void SetUniform(const std::string& name, const float v1, const float v2, const float v3, const float v4) override;

protected:
	sf::Shader _shader;
};