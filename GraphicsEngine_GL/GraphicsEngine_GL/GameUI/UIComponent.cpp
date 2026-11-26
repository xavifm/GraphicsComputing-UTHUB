#include "Object/GameObject/GameObject.h"
#include "RmlUi/Include/RmlUi/Core/Context.h"
#include "UIComponent.h"
#include "Globals.h"


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
        return;

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
        currentUIFile.clear();
        return;
    }

    if (path == currentUIFile && document)
        return;

    Rml::ElementDocument* newDocument = uiContext->LoadDocument(path);

    if (!newDocument)
    {
        std::cerr << "[UIComponent] Failed to load UI: "
                  << path << std::endl;
        return;
    }

    CloseDocument();

    document = newDocument;
    currentUIFile = std::move(path);

    UpdateUIVisibility(uiVisible);

    std::cout << "[UIComponent] Loaded UI: "
              << currentUIFile << std::endl;
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
