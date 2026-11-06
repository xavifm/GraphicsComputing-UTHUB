#pragma once
#include "../Object/GameObject/GameObject.h"
#include "Component/SerializableComponent.h"

namespace Assets {
    class BoxCollider;
}

class BoxCollider : public SerializableComponent<BoxCollider>
{
public:
    BoxCollider();
    ~BoxCollider();

    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "BoxCollider";
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

private:
    template<class Self, class Visitor>
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("width", self.width);
        visitor("height", self.height);
        visitor("depth", self.depth);
    }
};