#pragma once

#include <Atlas/Layer.h>

namespace Atlas {

class ImGuiLayer : public Layer {
   public:
    ImGuiLayer();
    virtual ~ImGuiLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate() override;

    virtual void onEvent(Event& event) override;

   private:
    float m_time = 0.0f;
};

}  // namespace Atlas