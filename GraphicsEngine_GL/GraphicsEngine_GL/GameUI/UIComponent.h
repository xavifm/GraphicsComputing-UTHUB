#pragma once
#include "Parameters/Mathx.h"
#include "Component/SerializableComponent.h"
#include <RmlUi/Include/RmlUi/Core/Core.h>
#include "RmlUi/Include/RmlUi/Core/ElementDocument.h"

namespace Assets {
    class UIComponent; //<- name of the class always
}

class UIComponent : public SerializableComponent<UIComponent> {
public:
    UIComponent();
    ~UIComponent();

    std::string uiFilePath;
    std::string currentUIFile;

    Rml::Context* uiContext = nullptr;
    Rml::ElementDocument* document = nullptr;

    bool uiVisible = false;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "UIComponent";
    }

    template<class Visitor>
    void Fields(Visitor& visitor)
    {
        VisitFields(*this, visitor);
    }

    template<class Visitor>
    void Fields(Visitor& visitor) const
    {
        VisitFields(*this, visitor);
    }

    void Update();

    void UpdateUIVisibility(bool visible);

    void LoadUIFile(std::string path);

    void SetUIContext(Rml::Context* context);

private:
    template<class Self, class Visitor>
    //Here you should list all the variables visible to the editor
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("UI File Path (.rml file)", self.uiFilePath);
        visitor("Visible", self.uiVisible);
    }

    void CloseDocument();
};
