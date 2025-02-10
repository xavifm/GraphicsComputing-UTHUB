#include <iostream>
#include "Application.h"

#include "Controllers/Window/WindowController.h"
#include "Controllers/OpenGL/OpenGLController.h"
#include "Controllers/Input/InputController.h"
#include "Controllers/Worlds/WorldController.h"
#include "Controllers/Camera/CameraController.h"


int main()
{
    Application* App = new Application();;

    App->AddController(new WindowController());
    App->AddController(new OpenGLController());
    App->AddController(new WorldController());

    if (!App->Init())
    {
        std::cerr << "[MAIN] Error starting the engine." << std::endl;
        return -1;
    }

    bool run = true;
    while (run)
    {
        if (App->PreUpdate() == UPDATE_STOP)
            break;

        update_status status = App->Update();
        if (status == UPDATE_STOP || status == UPDATE_ERROR)
            break;

        if (App->PostUpdate() == UPDATE_STOP)
            break;
    }

    if (!App->CleanUp())
    {
        std::cerr << "[MAIN] error deleting memory." << std::endl;
        return -1;
    }

    return 0;
}
