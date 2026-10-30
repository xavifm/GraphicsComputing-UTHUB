#pragma once
#include <iosfwd>

class Object;
class GameObject;

class Component
{
public:
    virtual ~Component() = default;
    virtual void Update();
    virtual void Destroy();

    void SetOwner(Object* newOwner);
    Object* GetOwner() const;
    GameObject* GetAttachedGameObject() const;

    virtual const char* GetTypeName() const = 0;

    virtual void Save(std::ostream& file) const = 0;
    virtual void Load(std::istream& file) = 0;

protected:
    Object* owner = nullptr;
};
