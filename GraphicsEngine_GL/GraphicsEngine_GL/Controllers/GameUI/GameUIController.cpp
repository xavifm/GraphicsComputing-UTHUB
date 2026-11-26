#include "GameUIController.h"
#include <memory>
#include <iostream>
#include "RmlUi/Core/Context.h"

GameUIController::GameUIController() = default;

GameUIController::~GameUIController() = default;

bool GameUIController::Init()
{
    if (context)
        return true;

    renderer = std::make_unique<RenderInterface_GL3>();

    if (!static_cast<bool>(*renderer))
    {
        std::cerr
            << "[GameUIController] Failed to create GL3 renderer\n";

        renderer.reset();
        return false;
    }

    renderer->SetViewport(1280, 720);

    Rml::SetRenderInterface(renderer.get());

    if (!Rml::Initialise())
    {
        std::cerr
            << "[GameUIController] Failed to initialize RmlUi\n";

        Rml::SetRenderInterface(nullptr);
        renderer.reset();
        return false;
    }

    context = Rml::CreateContext(
        "GameUI",
        Rml::Vector2i(1280, 720)
    );

    if (!context)
    {
        std::cerr
            << "[GameUIController] Failed to create context\n";

        Rml::Shutdown();
        Rml::SetRenderInterface(nullptr);
        renderer.reset();
        return false;
    }

    if (!Rml::LoadFontFace(
        "./Assets/Lato-Regular.ttf"))
    {
        std::cerr
            << "[GameUIController] Failed to load font\n";
    }

    std::cout
        << "[GameUIController] RmlUi initialized\n";

    return true;
}


update_status GameUIController::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status GameUIController::Update()
{
    //if (context)
        //context->Update();

    return UPDATE_CONTINUE;
}

update_status GameUIController::PostUpdate()
{
    return UPDATE_CONTINUE;
}

bool GameUIController::CleanUp()
{
    Shutdown();
    return true;
}

void GameUIController::Render()
{
    if (!context || !renderer)
        return;

    GLint before = 0;
    GLint after = 0;

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &before);

    renderer->SetViewport(1280, 720);
    renderer->BeginFrame();

    context->Render();

    renderer->EndFrame();

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &after);

    static bool printed = false;
    if (!printed)
    {
        std::cout << "[GameUI] FBO before: "
                  << before << '\n';

        std::cout << "[GameUI] FBO after: "
                  << after << '\n';

        std::cout << "[GameUI] Documents: "
                  << context->GetNumDocuments() << '\n';

        printed = true;
    }
}

void GameUIController::Shutdown()
{
    if (context)
    {
        Rml::RemoveContext(context->GetName());
        context = nullptr;
    }
}

Rml::Context* GameUIController::GetContext()
{
    return context;
}
