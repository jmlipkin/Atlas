#pragma once

extern Atlas::Application* Atlas::createApplication();

int main(int argc, char** argv) {
    Atlas::Log::init();

    AT_CORE_INFO("Initialized Log!");

    auto app = Atlas::createApplication();
    app->run();
    delete app;
}