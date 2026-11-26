#include "Object/GameObject/GameObject.h"
#include "RmlUi/Include/RmlUi/Core/Context.h"
#include "UIComponent.h"
#include "Globals.h"
#include "WorldController.h"


UIComponent::UIComponent()
{
}



UIComponent::~UIComponent()
{
    CloseDocument();
}

void UIComponent::Update()
{
    SerializableComponent<UIComponent>::Update();

    if (!uiContext)
    {
        uiContext = GetAttachedGameObject()->worldController->GetGameUIController()->GetContext();
        return;
    }

    if (currentUIFile != uiFilePath)
        LoadUIFile(uiFilePath);
}

void UIComponent::UpdateUIVisibility(bool visible)
{
    uiVisible = visible;

    if (!document)
        return;

    if (visible)
        document->Show();
    else
        document->Hide();
}

void UIComponent::LoadUIFile(std::string path)
{
    if (!uiContext)
    {
        std::cerr << "[UIComponent] Context not initialized\n";
        return;
    }

    if (path.empty())
    {
        CloseDocument();
        return;
    }

    if (path == currentUIFile && document)
        return;

    namespace fs = std::filesystem;

    fs::path fullPath = fs::path("Assets") / path;

    if (!fs::exists(fullPath))
    {
        std::cerr << "[UIComponent] File not found: "
                  << fs::absolute(fullPath) << '\n';
        return;
    }

    Rml::ElementDocument* newDocument =
        uiContext->LoadDocument(fullPath.string());

    if (!newDocument)
    {
        std::cerr << "[UIComponent] Failed to load UI: "
                  << fullPath << '\n';
        return;
    }

    CloseDocument();

    document = newDocument;
    currentUIFile = path;

    UpdateUIVisibility(uiVisible);

    std::cout << "[UIComponent] UI loaded: "
              << fullPath << '\n';
}

void UIComponent::SetUIContext(Rml::Context* context)
{
    if (uiContext == context)
        return;

    CloseDocument();

    uiContext = context;
    currentUIFile.clear();
}

void UIComponent::CloseDocument()
{
    if (document)
    {
        document->Close();
        document = nullptr;
    }

    currentUIFile.clear();
}
