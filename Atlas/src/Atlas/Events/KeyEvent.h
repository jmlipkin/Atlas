#pragma once

#include "Event.h"

namespace Atlas {
class KeyEvent : public Event {
   public:
    inline int getKeyCode() const { return m_keyCode; }

    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_INPUT | EVENT_CATEGORY_KEYBOARD);

   protected:
    KeyEvent(int keycode) : m_keyCode(keycode) {}

    int m_keyCode;
};

class KeyPressedEvent : public KeyEvent {
   public:
    KeyPressedEvent(int keycode, int repeatCount)
        : KeyEvent(keycode), m_repeatCount(repeatCount) {}

    inline int getRepeatCount() const { return m_repeatCount; }

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KEY_PRESSED)
   private:
    int m_repeatCount;
};

class KeyTypedEvent : public KeyEvent {
    public:
    KeyTypedEvent(int keycode) : KeyEvent(keycode) {}
    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << m_keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KEY_TYPED)
};

class KeyReleasedEvent : public KeyEvent {
    public:
    KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_keyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KEY_RELEASED);
};

}  // namespace Atlas