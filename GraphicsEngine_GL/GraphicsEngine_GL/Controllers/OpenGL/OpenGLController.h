#pragma once
#include "../Controller.h"

class OpenGLController : public Controller
{
public:
    OpenGLController();
    ~OpenGLController();

    bool Init() override;
    update_status PreUpdate() override;
    update_status PostUpdate() override;
    bool CleanUp() override;
    void WindowResized(unsigned width, unsigned height);

private:
    bool contextCreated = false;
};
