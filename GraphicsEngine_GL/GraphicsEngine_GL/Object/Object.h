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

    std::string& GetName();
    void SetName(std::string _name);

    void AddComponent(Component* component);

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        static_assert(
            std::is_base_of_v<Component, T>,
            "T should inherit from Component"
        );

        if (T* existing = GetComponent<T>())
        {
            return existing;
        }

        T* component = new T(std::forward<Args>(args)...);

        AddComponent(component);

        return component;
    }

    template <typename T>
    T* GetComponent()
    {
        if (!std::is_base_of_v<Component, T>)
        {
            std::cout << "T Should hederate from Component!" << std::endl;
            return nullptr;
        }

        for (auto& component : components)
        {
            if (T* result = dynamic_cast<T*>(component))
            {
                return result;
            }
        }

        return nullptr;
    }

    template <typename T>
    std::vector<T*> GetComponents()
    {
        if (!std::is_base_of_v<Component, T>)
        {
            std::cout << "T Should hederate from Component!" << std::endl;
            return nullptr;
        }

        std::vector<T*> result;

        for (auto& component : components)
        {
            if (T* casted = dynamic_cast<T*>(component))
            {
                result.push_back(casted);
            }
        }

        return result;
    }

    std::vector<Component*>& GetFullComponentsList() { return components; }

    virtual void Start();

    virtual void Update(float _deltaTime = 0.0f);

    virtual void Destroy();

private:
    std::string name;
    std::vector<Component*> components;
};