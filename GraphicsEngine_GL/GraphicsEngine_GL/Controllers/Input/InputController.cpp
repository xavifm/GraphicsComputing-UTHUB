#include "InputController.h"
#include <GLFW/glfw3.h>
#include <iostream>

InputController::InputController() : MouseX(0), MouseY(0), LastMouseX(0), LastMouseY(0) 
{
}

InputController::~InputController() 
{
}

bool InputController::Init()
{
    return true;
}

update_status InputController::PreUpdate()
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) return UPDATE_CONTINUE;

    double x, y;
    glfwGetCursorPos(window, &x, &y);
    MouseX = static_cast<float>(x);
    MouseY = static_cast<float>(y);

    MouseMotion.x = MouseX - LastMouseX;
    MouseMotion.y = MouseY - LastMouseY;

    LastMouseX = MouseX;
    LastMouseY = MouseY;

    return UPDATE_CONTINUE;
}

update_status InputController::Update()
{
    return UPDATE_CONTINUE;
}

bool InputController::CleanUp()
{
    return true;
}

bool InputController::GetKeyboardKey(int _key, int _pressMode)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) return false;

    int state = glfwGetKey(window, _key);
    return (state == _pressMode);
}

bool InputController::GetMouseButton(int _key, int _pressMode)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (!window) return false;

    int state = glfwGetMouseButton(window, _key);
    return (state == _pressMode);
}
