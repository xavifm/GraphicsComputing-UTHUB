#include "ImguiController.h"

#include <iostream>
#include "Controllers/Window/WindowController.h"

ImguiController::ImguiController(WindowController* controller)
{
    windowController = controller;
}

ImguiController::~ImguiController() {}

bool ImguiController::Init()
{
    if (windowController == nullptr)
    {
        std::cout << "ImguiController: windowController nullptr\n";
        return false;
    }

    window = windowController->GetWindow();
    if (window == nullptr)
    {
        std::cout << "ImguiController: GLFWwindow nullptr\n";
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        std::cout << "ImguiController: ImGui_ImplGlfw_InitForOpenGL failed\n";
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 330"))
    {
        std::cout << "ImguiController: ImGui_ImplOpenGL3_Init failed\n";
        return false;
    }

    std::cout << "ImguiController: Init OK\n";
    return true;
}

update_status ImguiController::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status ImguiController::PostUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Quick Test");
    ImGui::Text("It works!");

    static float value = 0.5f;
    ImGui::SliderFloat("value", &value, 0.0f, 1.0f);

    if (ImGui::Button("Button"))
        std::cout << "Button pressed\n";

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return UPDATE_CONTINUE;
}

bool ImguiController::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    std::cout << "ImguiController: CleanUp OK\n";
    return true;
}