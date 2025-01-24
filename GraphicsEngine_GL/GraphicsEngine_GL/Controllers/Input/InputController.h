#pragma once

#include "../Controller.h"

class InputController : public Controller
{
public:

    InputController();
    ~InputController();

    bool Init();
    update_status PreUpdate();
    update_status Update();
    bool CleanUp();

    bool GetKeyboardKey(int _key, int _pressMode);
    bool GetMouseButton(int _key, int _pressMode);

    const Vector2D GetMouseMotion() const { return MouseMotion; }

private:

    float MouseX;
    float MouseY;

    float LastMouseX;
    float LastMouseY;
    
    Vector2D MouseMotion;
    Vector2D MousePosition;
};
