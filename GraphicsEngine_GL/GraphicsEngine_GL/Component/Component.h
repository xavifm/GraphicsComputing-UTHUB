#pragma once

class Object;

class Component
{
public:
    virtual ~Component() = default;
    virtual void Update();
    virtual void Destroy();

    void SetOwner(Object* newOwner);
    Object* GetOwner() const;

protected:
    Object* owner = nullptr;
};
