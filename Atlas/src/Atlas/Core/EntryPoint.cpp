#include "Atlas/Core/EntryPoint.h"

int main(int argc, char** argv) {
	Atlas::Log::init();

	char welcome_msg[] =
		"=====================================\n\n"
		"        WELCOME TO ATLAS ENGINE      \n\n"
		"=====================================\n\n";
	AT_LOG(welcome_msg);

    AT_PROFILE_BEGIN_SESSION("Startup", "AtlasProfile.Startup.json");
    auto app = Atlas::createApplication();
    AT_PROFILE_END_SESSION();

    AT_PROFILE_BEGIN_SESSION("Runtime", "AtlasProfile.RunTime.json");
    app->run();
    AT_PROFILE_END_SESSION();

    AT_PROFILE_BEGIN_SESSION("Shutdown", "AtlasProfile.Shutdown.json");
    delete app;
    AT_PROFILE_END_SESSION();
}