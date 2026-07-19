#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Component/Component.h"

class Object
{
public:
    Object() = default;
    virtual ~Object() = default;

    const std::string& GetName() const noexcept;
    void SetName(std::string _name);

    void AddComponent(std::unique_ptr<Component> _component);

    template <typename T>
    const T* GetComponent() const noexcept
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
    std::vector<const T*> GetComponents() const noexcept
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

    const std::vector<std::unique_ptr<Component>>& GetFullComponentsList() { return components; }

    virtual void Start();

    virtual void Update(float _deltaTime = 0.0f);

    virtual void Destroy();

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};