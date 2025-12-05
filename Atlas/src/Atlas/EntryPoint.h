#pragma once

extern Atlas::Application* Atlas::createApplication();

int main(int argc, char** argv) {
    auto app = Atlas::createApplication();
    app->run();
    delete app;
}