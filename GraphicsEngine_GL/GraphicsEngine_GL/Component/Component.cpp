#include "Component.h"

void Component::Update()
{
}

void Component::Destroy()
{
}

void Component::SetOwner(Object *newOwner)
{
    this->owner = newOwner;
}

Object * Component::GetOwner() const
{
    return this->owner;
}
