#pragma once


#include "FrameBufferController.h"
#include "ThirdParty/include/imgui/imgui.h"
#include "ThirdParty/include/imgui/imgui_impl_glfw.h"
#include "ThirdParty/include/imgui/imgui_impl_opengl3.h"
#include "WindowController.h"
#include "WorldController.h"
#include "../Controller.h"
#include <filesystem>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <system_error>

namespace fs = std::filesystem;

class ImguiController : public Controller
{
public:
    ImguiController(WindowController* controller, FrameBufferController* frameBuffer, WorldController* controllerWorld);
    ~ImguiController();

    bool Init() override;
    update_status PreUpdate() override;
    update_status PostUpdate() override;

    void ReplaceAll(std::string &text, const std::string &from, const std::string &to);

    void DrawAssetBrowser();

    bool CleanUp() override;

private:
    bool contextCreated = false;
    WindowController* windowController;
    FrameBufferController* frameBufferController;
    WorldController* worldController;
    GameObject* selectedGameObject = nullptr;
    GLFWwindow* window;
};