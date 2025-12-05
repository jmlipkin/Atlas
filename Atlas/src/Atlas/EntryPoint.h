#pragma once

extern Atlas::Application* Atlas::createApplication();

int main(int argc, char** argv) {
    Atlas::Log::init();
    
    AT_CORE_WARN("Initialized Log!");
	int a = 5;
	AT_INFO("Hello! Var={0}", a);

    auto app = Atlas::createApplication();
    app->run();
    delete app;
}