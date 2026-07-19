#pragma once
#include "Globals.h"
#include "Object/Object.h"

class GameObject : public Object
{
public:
    Vector3D position;
    Vector3D rotation;
    Vector3D size;

    GameObject(const Vector3D& _position, const Vector3D& _size) { position = _position; size = _size; }

    void Start() override;
    void Update(float _deltaTime) override;
    void Destroy() override;
};
