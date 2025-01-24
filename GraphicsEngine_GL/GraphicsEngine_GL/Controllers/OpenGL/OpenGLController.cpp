#include "OpenGLController.h"
#include <glad/glad.h>
#include <iostream>

#include "../Window/WindowController.h"

OpenGLController::OpenGLController()
{
}

OpenGLController::~OpenGLController()
{
}

bool OpenGLController::Init()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Error starting GLAD!" << std::endl;
        return false;
    }
    contextCreated = true;

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    return true;
}

update_status OpenGLController::PreUpdate()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return UPDATE_CONTINUE;
}

update_status OpenGLController::PostUpdate()
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window)
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return UPDATE_CONTINUE;
}

bool OpenGLController::CleanUp()
{
    return true;
}

void OpenGLController::WindowResized(unsigned width, unsigned height)
{
    glViewport(0, 0, width, height);
}
