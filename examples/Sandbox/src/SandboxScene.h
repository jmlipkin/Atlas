#pragma once

#include <Atlas.h>

class SandboxScene : public Atlas::Scene {
   public:
    SandboxScene() {
        m_rectangle = createEntity("Rectangle");
    }
    ~SandboxScene() = default;

    virtual void onUpdate(Atlas::DeltaTime dt) override {}

   private:
    Atlas::Entity m_rectangle;
};