#include "atpch.h"
#include "Behavior.h"

namespace Atlas {

void Behavior::generateNewScript(const std::string &path) {
	std::string	  name = std::filesystem::path(path).stem().string();
	std::ofstream file(path);
	std::string	  scriptTemplate =
R"x(#pragma once

#include <Atlas.h>

namespace Atlas {

class )x" + name + R"x( : public Atlas::Behavior {
  public:
    AT_BEHAVIOR()x" +
                            name + R"x()

    virtual void onCreate() override {}
    virtual void onDestroy() override {}
    virtual void onUpdate(DeltaTime dt) override {}
    virtual void onEvent(Event& event) override {}

    virtual void exposeProperties() override {
        // Add any variables that you want visible in the editor here using the
        // expose() function.
        // Usage: expose(<variable name>, <value>);
        expose("My Variable", example_variable);
    }

  private:
    // add member variables here
    float example_variable = 0.0f;
};

AT_REGISTER_SCRIPT()x" +
                            name + R"x()

}  // namespace Atlas)x";

	file << scriptTemplate << std::endl;
	file.close();
}
}  // namespace Atlas