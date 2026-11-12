#pragma once
#include "CameraController.h"
#include "Component/SerializableComponent.h"

namespace Assets {
    class CameraComponent; //<- name of the class always
}

class CameraComponent : public SerializableComponent<CameraComponent> {
public:
    CameraComponent();
    ~CameraComponent();

    FrameBufferController* frameBufferController;
    CameraController* cameraController;

    float localX = 0.f;
    float localY = 0.f;
    float localZ = 0.f;

    float localRX = 0.f;
    float localRY = 0.f;
    float localRZ = 0.f;

    float positionX = 0.f;
    float positionY = 0.f;
    float positionZ = 0.f;

    float rotationX = 0.f;
    float rotationY = 0.f;
    float rotationZ = 0.f;

    bool Enabled = true;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "CameraComponent";
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
    //Here you should list all the variables visible to the editor
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("localX", self.localX);
        visitor("localY", self.localY);
        visitor("localZ", self.localZ);
        visitor("rotationX", self.localRX);
        visitor("rotationY", self.localRY);
        visitor("rotationZ", self.localRZ);
        visitor("Enabled", self.Enabled);
    }
};

