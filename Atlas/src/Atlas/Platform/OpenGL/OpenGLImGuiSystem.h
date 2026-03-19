#pragma once

#include "Atlas/ImGui/ImGuiSystem.h"

namespace Atlas {

class OpenGLImGuiSystem : public ImGuiSystem {
   public:
    virtual void initImGuiLayer() override;
	virtual void cleanImGuiLayer() override;

    // todo
    virtual void rescale(float newScale) override {}
};

}  // namespace Atlas