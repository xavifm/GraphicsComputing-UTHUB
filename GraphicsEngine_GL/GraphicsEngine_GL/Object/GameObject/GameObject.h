#pragma once
#include "Globals.h"
#include "ImguiController.h"
#include "Object/Object.h"

class WorldController;

class GameObject : public Object
{
public:
    Vector3D position;
    Vector3D rotation;
    Vector3D size;
    GameObject* parent = nullptr;
    WorldController* worldController = nullptr;
    std::string parentName = "";
    Mat4x4 modelMatrix;

    GameObject(const Vector3D& _position, const Vector3D& _size, WorldController* _worldController) { position = _position; size = _size; worldController = _worldController; };

    void Start() override;
    void Update(float _deltaTime) override;
    void Destroy() override;

    void UpdateModelMatrix();
};
