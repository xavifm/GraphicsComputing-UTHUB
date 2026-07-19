#pragma once

class Component
{
public:
    virtual ~Component() = default;
    virtual void Update();
    virtual void Destroy();
};