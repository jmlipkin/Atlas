#include "atpch.h"
#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Atlas/Log.h"

namespace Atlas {

    Application::Application() {}

    Application::~Application() {}

    void Application::run() {
        WindowResizeEvent e(1280, 720);
        if (e.isInCategory(EVENT_CATEGORY_APPLICATION))
            AT_TRACE(e.toString());
        if(e.isInCategory(EVENT_CATEGORY_INPUT))
            AT_TRACE(e.toString());

        while (true) {
        }
    }
}