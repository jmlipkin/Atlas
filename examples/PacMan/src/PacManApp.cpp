#include <Atlas.h>

class PacMan : public Atlas::Application {

    public:
    PacMan() {}
    ~PacMan() override {}
};

Atlas::Application* Atlas::createApplication() {
    return new PacMan;
}