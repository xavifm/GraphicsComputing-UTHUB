#include "WindowController.h"

WindowController::WindowController()
{
}

WindowController::~WindowController()
{
}

bool WindowController::Init()
{
    if (!glfwInit())
    {
        std::cerr << "Error starting GLFW!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GraphicsEngineGL", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Error creating the window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    initialized = true;
    return true;
}


update_status WindowController::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status WindowController::Update()
{
    if (glfwWindowShouldClose(window))
    {
        return UPDATE_STOP;
    }
    return UPDATE_CONTINUE;
}

bool WindowController::CleanUp()
{
    if (initialized)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
        initialized = false;
    }
    return true;
}
