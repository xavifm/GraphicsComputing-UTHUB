#include <iostream>
#include "Application.h"

#include "Controllers/Window/WindowController.h"
#include "Controllers/OpenGL/OpenGLController.h"
#include "Controllers/Input/InputController.h"
#include "Controllers/Worlds/WorldController.h"

int main()
{
    Application app;

    app.AddController(new WindowController());
    app.AddController(new OpenGLController());
    app.AddController(new InputController());
    app.AddController(new WorldController());

    if (!app.Init())
    {
        std::cerr << "[MAIN] Error starting the engine." << std::endl;
        return -1;
    }

    bool run = true;
    while (run)
    {
        if (app.PreUpdate() == UPDATE_STOP)
            break;

        update_status status = app.Update();
        if (status == UPDATE_STOP || status == UPDATE_ERROR)
            break;

        if (app.PostUpdate() == UPDATE_STOP)
            break;
    }

    if (!app.CleanUp())
    {
        std::cerr << "[MAIN] error deleting memory." << std::endl;
        return -1;
    }

    return 0;
}
