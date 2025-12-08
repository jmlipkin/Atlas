#pragma once

#include "Events/Event.h"

namespace Atlas {

class Layer {
   public:
    Layer(const std::string& name) : m_name(name) {}
    virtual ~Layer() = default;

    virtual void onAttach() = 0;
    virtual void onDetach() = 0;
    virtual void onUpdate() = 0;

    virtual void onEvent(Event& event) = 0;

    inline const std::string& getName() const { return m_name; }

   protected:
    std::string m_name;
};

}  // namespace Atlas