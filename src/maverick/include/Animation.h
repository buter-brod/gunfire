#pragma once
#include <string>
#include <vector>
#include <memory>

const static unsigned int defaultFPS = 30;

class Animation {

public:
	Animation(const std::string& name, const unsigned int size, const unsigned int fps = defaultFPS);
	unsigned int GetSize() const { return _size; }
	unsigned int GetFPS() const { return _fps; }
	const std::string& getName() const { return _name; }

	const std::string& GetFrameName(const float dt) const;

protected:
	unsigned int getFrameIndex(const float dt) const;

private:
	std::string _name;
	unsigned int _size{ 1 };
	unsigned int _fps{ defaultFPS };

	mutable std::vector<std::string> _frames;
};

typedef std::shared_ptr<Animation> AnimationPtr;
typedef std::weak_ptr<Animation> AnimationWPtr;