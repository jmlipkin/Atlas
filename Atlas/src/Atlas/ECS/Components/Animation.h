#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace Atlas {

struct AnimationFrame {
	glm::ivec2 index;
};

struct AnimationClip {
	std::string name;

	std::string texturePath;
	glm::ivec2	sizeInTiles;

	bool  shouldLoop = false;
	float frameRate	 = 12.0f;

	std::vector<AnimationFrame> frames;

	double timeSinceLastFrame = 0.0;  // internal only!
	size_t currentFrame		  = 0;	  // internal only!
	bool   playing			  = false;
};

namespace Component {

struct Animations {
	std::unordered_map<std::string, AnimationClip> clips;

	std::string activeClip;

	bool containsActiveClip() const { return !activeClip.empty() && clips.contains(activeClip); }
};

}  // namespace Component
}  // namespace Atlas