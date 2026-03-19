#pragma once

#include "Atlas/ImGui/ImGuiSystem.h"

namespace Atlas {

class MetalImGuiSystem : public ImGuiSystem {
   public:
    virtual void initImGuiLayer() override;
	virtual void cleanImGuiLayer() override;

    virtual void rescale(float newScale) override;
};

}  // namespace Atlas