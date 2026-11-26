#pragma once

#include <RmlUi/Include/RmlUi/Core/Core.h>
#include <RmlUi/Backends/RmlUi_Renderer_GL3.h>
#include "Controller.h"

class GameUIController : public Controller
{
public:
    GameUIController();
    ~GameUIController() override;

    bool Init() override;

    update_status PreUpdate() override;
    update_status Update() override;
    update_status PostUpdate() override;

    bool CleanUp() override;

    void Render();
    void Shutdown();

    Rml::Context* GetContext();

private:
    Rml::Context* context = nullptr;
    std::unique_ptr<RenderInterface_GL3> renderer;
};
