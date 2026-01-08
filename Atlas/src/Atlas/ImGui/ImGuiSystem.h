#pragma once

#include "Atlas/Renderer/RendererAPI.h"

namespace Atlas {

class ImGuiSystem {
   public:
    virtual ~ImGuiSystem() = default;
    
    virtual void initImGuiLayer() = 0;
    virtual void cleanImGuiLayer() = 0;

    static std::unique_ptr<ImGuiSystem> create();
};
}  // namespace Atlas