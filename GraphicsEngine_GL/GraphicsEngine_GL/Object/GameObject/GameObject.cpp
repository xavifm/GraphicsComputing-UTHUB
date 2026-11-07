#include "GameObject.h"

void GameObject::Start()
{
    Object::Start();
}

void GameObject::Update(float _deltaTime)
{
    Object::Update(_deltaTime);

    for (auto& component : GetFullComponentsList())
    {
        component->Update();
    }
}

void GameObject::Destroy()
{
    Object::Destroy();

    for (auto& component : GetFullComponentsList())
    {
        component->Destroy();
    }
}

void GameObject::UpdateModelMatrix()
{
    const Mat4x4 translation = Mat4x4::Translate(position);

    const Mat4x4 rotationX = Mat4x4::RotateX(rotation.x);
    const Mat4x4 rotationY = Mat4x4::RotateY(rotation.y);
    const Mat4x4 rotationZ = Mat4x4::RotateZ(rotation.z);

    const Mat4x4 scale = Mat4x4::Scale(size);

    modelMatrix =
        translation *
        rotationX *
        rotationY *
        rotationZ *
        scale;
}
