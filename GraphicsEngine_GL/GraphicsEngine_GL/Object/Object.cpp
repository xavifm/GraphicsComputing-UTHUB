#include "Object.h"
#include <iostream>

const std::string& Object::GetName() const noexcept
{
    return name;
}

void Object::SetName(std::string _name)
{
    name = std::move(_name);
}

void Object::AddComponent(std::unique_ptr<Component> _component)
{
    if (!_component)
    {
        std::cout << "Component is nullptr" << std::endl;
    }

    components.push_back(std::move(_component));
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