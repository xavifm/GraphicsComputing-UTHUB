#pragma once
#include "Component/SerializableComponent.h"

namespace Assets {
    class ExampleComponent; //<- name of the class always
}

class ExampleComponent : public SerializableComponent<ExampleComponent> {
public:
    ExampleComponent();
    ~ExampleComponent();

    std::string test;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "ExampleComponent";
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

private:
    template<class Self, class Visitor>
    //Here you should list all the variables visible to the editor
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("test", self.test);
    }
};
