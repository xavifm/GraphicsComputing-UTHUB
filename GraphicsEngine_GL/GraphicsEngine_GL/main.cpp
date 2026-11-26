
#include <iostream>
#include <algorithm>

#include "Application.h"
#include <Generated/RegisterComponents.h>

#include "Controllers/Window/WindowController.h"
#include "Controllers/OpenGL/OpenGLController.h"
#include "Controllers/Input/InputController.h"
#include "Controllers/Worlds/WorldController.h"
#include "Controllers/Camera/CameraController.h"
#include "Controllers/UI/ImguiController.h"
#include "Controllers/Camera/FrameBufferController.h"
#include "Engine/EngineController.h"
#include "GameUI/GameUIController.h"

int main()
{
    Application* App = new Application();

    RegisterProjectComponents();

    auto* windowController = new WindowController();

    auto* openGLController = new OpenGLController();

    auto* frameBufferController =
        new FrameBufferController(0);

    auto* gameUIController = new GameUIController();

    auto* worldController = new WorldController(
        frameBufferController,
        gameUIController
    );

    auto* imguiController = new ImguiController(
        windowController,
        frameBufferController,
        worldController
    );

    auto* engineController =
        new EngineController(imguiController);

    App->AddController(windowController);
    App->AddController(openGLController);
    App->AddController(frameBufferController);
    App->AddController(worldController);
    App->AddController(imguiController);
    App->AddController(engineController);

    auto& controllers = App->GetControllers();

    std::swap(controllers[1], controllers[4]);
    std::swap(controllers[4], controllers[2]);

    controllers.insert(
        controllers.begin() + 3,
        gameUIController
    );

    if (!App->Init())
    {
        std::cerr
            << "[MAIN] Error starting the engine."
            << std::endl;

        delete App;
        return -1;
    }

    while (true)
    {
        update_status status = App->PreUpdate();

        if (status == UPDATE_STOP ||
            status == UPDATE_ERROR)
            break;

        status = App->Update();

        if (status == UPDATE_STOP ||
            status == UPDATE_ERROR)
            break;

        status = App->PostUpdate();

        if (status == UPDATE_STOP ||
            status == UPDATE_ERROR)
            break;
    }

    if (!App->CleanUp())
    {
        std::cerr
            << "[MAIN] Error cleaning up engine."
            << std::endl;

        delete App;
        return -1;
    }

    delete App;

    return 0;
}
