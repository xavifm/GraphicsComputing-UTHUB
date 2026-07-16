#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Component/Component.h"

class Object
{
public:
    const std::string& GetName() const noexcept;
    void SetName(std::string _name);

    void AddComponent(std::unique_ptr<Component> _component);

    template<typename T>
    const T* GetComponent() const noexcept;

    template<typename T>
    std::vector<const T*> GetComponents() const noexcept;

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};