#pragma once

extern Atlas::Application* Atlas::createApplication();

int main(int argc, char** argv) {
    std::cout
        << "================================================================================\n\n"
        << "                              WELCOME TO ATLAS ENGINE                            \n\n"
        << "================================================================================\n\n";
    Atlas::Log::init();

    AT_CORE_TRACE("Logging initialized");

    auto app = Atlas::createApplication();
    app->run();
    delete app;
}