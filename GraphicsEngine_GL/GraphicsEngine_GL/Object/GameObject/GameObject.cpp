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
