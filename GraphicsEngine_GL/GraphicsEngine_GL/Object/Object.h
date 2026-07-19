#pragma once

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

    template<typename T>
    const T* GetComponent() const noexcept;

    template<typename T>
    std::vector<const T*> GetComponents() const noexcept;

    const std::vector<std::unique_ptr<Component>>& GetFullComponentsList() { return components; }

    virtual void Start();

    virtual void Update(float _deltaTime = 0.0f);

    virtual void Destroy();

private:
    std::string name;
    std::vector<std::unique_ptr<Component>> components;
};