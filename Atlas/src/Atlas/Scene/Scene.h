#pragma once

#include "Atlas/Scene/Registry.h"

namespace Atlas {

class Scene {
   private:
    Registry m_registry;
    friend class Entity;
};

}  // namespace Atlas