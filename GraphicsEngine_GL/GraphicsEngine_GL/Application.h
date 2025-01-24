#pragma once
#include <vector>
#include <iostream>
#include "Controllers/Controller.h"
#include "Parameters/Globals.h"

class Application
{
public:
    Application() {}
    ~Application() {}

    bool Init();
    update_status PreUpdate();
    update_status Update();
    update_status PostUpdate();
    bool CleanUp();

    void AddController(Controller* controller);

private:
    std::vector<Controller*> controllers;
};