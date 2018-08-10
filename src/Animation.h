#pragma once
#include <string>
#include <vector>
#include "Graphics.h"

const static unsigned int defaultFPS = 30;

class Animation {

public:
	Animation(const std::string& name, unsigned int size, unsigned int fps = defaultFPS);

protected:
	bool load(const std::string& name, unsigned int size, unsigned int fps);

private:
	std::string _name;
	std::vector<Sprite> _frames;
	unsigned int _fps{ defaultFPS };
};