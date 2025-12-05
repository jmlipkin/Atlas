#pragma once

#include "Event.h"

namespace Atlas {
class MouseMovedEvent : public Event {
   public:
    MouseMovedEvent(float x, float y) : m_mouseX(x), m_mouseY(y) {}

    inline float getX() const { return m_mouseX; }
    inline float getY() const { return m_mouseY; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << m_mouseX << ", " << m_mouseY;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MOUSE_MOVED)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

   private:
    float m_mouseX, m_mouseY;
};

class MouseScrolledEvent : public Event {
   public:
    MouseScrolledEvent(float xOffset, float yOffset) : m_offsetX(xOffset), m_offsetY(yOffset) {}

    inline float getXOffset() const { return m_offsetX; }
    inline float getYOffset() const { return m_offsetY; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
        return ss.str();
    }

    EVENT_CLASS_TYPE(MOUSE_SCROLLED)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

   private:
    float m_offsetX, m_offsetY;
};

class MouseButtonEvent : public Event {
   public:
    inline int getMouseButton() const { return m_button; }

    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
   protected:
    MouseButtonEvent(int button) : m_button(button) {}

    int m_button;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
   public:
    MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonPressedEvent: " << m_button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
   public:
    MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "MouseButtonReleasedEvent: " << m_button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)
};
}  // namespace Atlas