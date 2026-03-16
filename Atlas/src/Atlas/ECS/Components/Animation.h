#pragma once

#include "Atlas/Renderer/SubTexture.h"

#include <vector>

namespace Atlas {

struct AnimationClip {
    const std::vector<std::shared_ptr<SubTexture>> frames;

    AnimationClip(std::vector<std::shared_ptr<SubTexture>> frameData) : frames(frameData) {}
};

}  // namespace Atlas

namespace Atlas::Component {

struct Animation {
    std::shared_ptr<AnimationClip> clip = nullptr;
    double timeSinceLastFrame = 0.0f;    // internal only!
    size_t nextFrame = 0;               // internal only!

    bool playing = false;
    float animationSpeed = 1.0f;
    bool shouldLoop = false;
    
    Animation(std::shared_ptr<AnimationClip> frameData) : clip(frameData) {}
    Animation(std::vector<std::shared_ptr<SubTexture>> frameData) : clip(std::make_shared<AnimationClip>(frameData)) {}
};

}  // namespace Atlas::Component