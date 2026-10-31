#include "Globals.h"
#include "Component/SerializableComponent.h"

class Light  : public SerializableComponent<Light>
{
public:
    Light();

    ~Light();

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "Light";
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
    bool StartedLight = false;

    int Id;
    Vector3D Position;
    Vector3D Color;

    template<class Self, class Visitor>
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("Color", self.Color);
    }
};
