#pragma once
#include <memory>
#include <SFML/Graphics/Shader.hpp>

class Shader {

public:

	explicit Shader(const std::string& name);
	sf::Shader* get() { return &_shader; }
	bool Load(const std::string& vF, const std::string& fF);

private:
	std::string _name;

	std::string _vFilename;
	std::string _fFilename;

	sf::Shader _shader;
};

typedef std::shared_ptr<Shader> ShaderPtr;