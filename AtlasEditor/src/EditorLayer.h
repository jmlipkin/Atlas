#pragma once

#include <Atlas.h>

namespace Atlas {
class EditorLayer : public Layer {
    public:
     EditorLayer() : Layer("Editor") {}

    virtual void onAttach() override {}
    virtual void onDetach() override {}
    virtual void onUpdate(DeltaTime dt) override {}

    virtual void onEvent(Event& event) override {}
};

}  // namespace Atlas