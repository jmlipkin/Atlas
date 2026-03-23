#pragma once

#include <Atlas.h>

namespace Atlas {

class HelloTess : public Atlas::Behavior {
public:
    AT_BEHAVIOR(HelloTess)

    virtual void onCreate() override {}
    virtual void onDestroy() override {}
    virtual void onUpdate(DeltaTime dt) override {}
    virtual void onEvent(Event& event) override {}

    virtual void exposeProperties() override {
        // Add any variables that you want visible in the editor here using the
        // expose() function.
        // Usage: expose(<variable name>, <value>);
        expose("My Variable ", example_variable);
}

private:
// add member variables here
float example_variable;
};

AT_REGISTER_SCRIPT(HelloTess)

}  // namespace Atlas
