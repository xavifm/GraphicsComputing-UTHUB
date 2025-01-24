#pragma once
#include "../Controller.h"
#include <GLFW/glfw3.h>

class WindowController : public Controller
{
public:
    WindowController();
    virtual ~WindowController();

    bool Init() override;
    update_status PreUpdate() override;
    update_status Update() override;
    bool CleanUp() override;

    GLFWwindow* GetWindow() const { return window; }

private:
    GLFWwindow* window = nullptr;
    bool initialized = false;
};