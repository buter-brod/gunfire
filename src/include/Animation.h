#pragma once
#include <string>
#include <vector>
#include "Sprite.h"

const static unsigned int defaultFPS = 30;

class Animation {

public:
	Animation(const std::string& name, unsigned int size, unsigned int fps = defaultFPS);
	unsigned int GetSize() const { return _size; }
	unsigned int GetFPS() const { return _fps; }
	const std::string& getName() const { return _name; }

	std::string GetFrameName(float dt) const;
	TextureRect GetTexRectFor(float dt);

protected:
	unsigned int getFrameIndex(float dt) const;

private:
	std::string _name;
	unsigned int _size{ 1 };
	unsigned int _fps{ defaultFPS };
};

typedef std::shared_ptr<Animation> AnimationPtr;
typedef std::weak_ptr<Animation> AnimationWPtr;