#pragma once

extern Atlas::Application* Atlas::createApplication();

int main(int argc, char** argv) {
    std::cout
        << "================================================================================\n\n"
        << "                              WELCOME TO ATLAS ENGINE                            \n\n"
        << "================================================================================\n\n";
    Atlas::Log::init();

    AT_CORE_TRACE("Logging initialized");

    AT_PROFILE_BEGIN_SESSION("Startup", "AtlasProfile-Startup.json");
    auto app = Atlas::createApplication();
    AT_PROFILE_END_SESSION();

    AT_PROFILE_BEGIN_SESSION("Runtime", "AtlasProfile-RunTime.json");
    app->run();
    AT_PROFILE_END_SESSION();

    AT_PROFILE_BEGIN_SESSION("Shutdown", "AtlasProfile-Shutdown.json");
    delete app;
    AT_PROFILE_END_SESSION();
}