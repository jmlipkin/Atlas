#pragma once

#include "Atlas/Core/Time.h"
#include "Atlas/Events/Event.h"

namespace Atlas {

class Layer {
   public:
    Layer(const std::string& name) : m_name(name) {}
    virtual ~Layer() = default;

    virtual void onAttach() = 0;
    virtual void onDetach() = 0;
	virtual void onUpdate(DeltaTime dt) = 0;
    virtual void onEvent(Event& event) = 0;
	
	virtual void onImGuiRender() {}

    inline const std::string& getName() const { return m_name; }

   protected:
    std::string m_name;
};

}  // namespace Atlas