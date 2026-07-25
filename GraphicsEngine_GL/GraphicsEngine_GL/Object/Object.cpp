#include "Object.h"
#include <algorithm>
#include <iostream>

std::string& Object::GetName()
{
    return name;
}

void Object::SetName(std::string _name)
{
    name = std::move(_name);
}

void Object::AddComponent(Component* component)
{
    if (component == nullptr)
    {
        std::cerr
            << "Object::AddComponent: component nullptr\n";
        return;
    }

    const auto iterator = std::find(components.begin(), components.end(), component);

    if (iterator != components.end())
    {
        std::cerr
            << "Object::AddComponent: component already added\n";
        return;
    }

    component->SetOwner(this);

    components.push_back(component);

    //component->Start();

    std::cout
        << "Component added"
        << " | Object: " << this
        << " | Component: " << component
        << " | Owner: " << component->GetOwner()
        << '\n';
}

void Object::Start()
{
}

void Object::Update(float _deltaTime)
{
}

void Object::Destroy()
{
    components.clear();
}