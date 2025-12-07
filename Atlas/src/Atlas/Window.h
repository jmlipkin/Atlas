#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "atpch.h"

namespace Atlas {

struct WindowProperties {
    std::string title;
    unsigned int width;
    unsigned int height;

    WindowProperties(const std::string& Title = "Atlas Engine", unsigned int Width = 1280, unsigned int Height = 720) : title(Title), width(Width), height(Height) {}
};

class Window {
   public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    virtual void onUpdate() = 0;

    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;

    virtual void* getNativeWindow() const = 0;

    // Window attributes
    virtual void setEventCallback(const EventCallbackFn& callback) = 0;
    virtual void setVSync(bool enabled) = 0;
    virtual bool isVSync() const = 0;

    static Window* create(const WindowProperties& properties = WindowProperties());
};
}  // namespace Atlas