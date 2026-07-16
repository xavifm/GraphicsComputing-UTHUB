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

template <typename T>
const T* Object::GetComponent() const noexcept
{
    if (!std::is_base_of_v<Component, T>)
    {
        std::cout << "T Should hederate from Component!" << std::endl;
        return nullptr;
    }

    for (const auto& component : components)
    {
        if (const T* result = dynamic_cast<const T*>(component.get()))
        {
            return result;
        }
    }

    return nullptr;
}

template <typename T>
std::vector<const T*> Object::GetComponents() const noexcept
{
    if (!std::is_base_of_v<Component, T>)
    {
        std::cout << "T Should hederate from Component!" << std::endl;
        return nullptr;
    }

    std::vector<const T*> result;

    for (const auto& component : components)
    {
        if (const T* casted = dynamic_cast<const T*>(component.get()))
        {
            result.push_back(casted);
        }
    }

    return result;
}