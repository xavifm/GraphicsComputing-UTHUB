#include "Application.h"

bool Application::Init()
{
    for (Controller* c : controllers)
    {
        if (!c->Init())
        {
            std::cerr << "Error starting a controller!" << std::endl;
            return false;
        }
    }
    return true;
}

update_status Application::PreUpdate()
{
    for (Controller* c : controllers)
    {
        update_status status = c->PreUpdate();

        if (status == UPDATE_STOP || status == UPDATE_ERROR)
            return status;
    }
    return UPDATE_CONTINUE;
}

update_status Application::Update()
{
    for (Controller* c : controllers)
    {
        update_status status = c->Update();

        if (status == UPDATE_STOP || status == UPDATE_ERROR)
            return status;
    }
    return UPDATE_CONTINUE;
}

update_status Application::PostUpdate()
{
    for (Controller* c : controllers)
    {
        update_status status = c->PostUpdate();

        if (status == UPDATE_STOP || status == UPDATE_ERROR)
            return status;
    }
    return UPDATE_CONTINUE;
}

bool Application::CleanUp()
{
    for (auto it = controllers.rbegin(); it != controllers.rend(); ++it)
    {
        if (!(*it)->CleanUp())
        {
            std::cerr << "Error removing a Controller!" << std::endl;
            return false;
        }
    }
    return true;
}

void Application::AddController(Controller* controller)
{
    controllers.push_back(controller);
}
