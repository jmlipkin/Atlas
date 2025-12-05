#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Atlas {

class Application {
   public:
    Application();
    virtual ~Application();

    void run();
};

// TO BE DEFINED IN CLIENT
Application* createApplication();

}  // namespace Atlas