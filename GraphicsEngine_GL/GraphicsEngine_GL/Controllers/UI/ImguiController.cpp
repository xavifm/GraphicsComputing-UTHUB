#include "ImguiController.h"

#include <iostream>
#include "Controllers/Window/WindowController.h"

ImguiController::ImguiController(WindowController* controller, FrameBufferController* frameBuffer)
{
    windowController = controller;
    frameBufferController = frameBuffer;
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

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Game Engine Editor", nullptr, flags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New Scene");
            ImGui::MenuItem("Open Scene");
            ImGui::MenuItem("Save Scene");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            ImGui::MenuItem("Create Empty");
            ImGui::MenuItem("Create Cube");
            ImGui::MenuItem("Create Camera");
            ImGui::MenuItem("Create Light");
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImVec2 available = ImGui::GetContentRegionAvail();

    float hierarchyWidth = 260.0f;
    float bottomHeight = 220.0f;

    ImGui::BeginChild("UpperArea", ImVec2(available.x, available.y - bottomHeight), true);

    ImGui::BeginChild("SceneHierarchy", ImVec2(hierarchyWidth, 0), true);
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();

    if (ImGui::TreeNode("Scene"))
    {
        ImGui::Selectable("Main Camera");
        ImGui::Selectable("Directional Light");
        ImGui::Selectable("Player");
        ImGui::Selectable("Enemy_01");
        ImGui::Selectable("Terrain");

        if (ImGui::TreeNode("Environment"))
        {
            ImGui::Selectable("Tree_01");
            ImGui::Selectable("Rock_01");
            ImGui::Selectable("House");
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Viewport", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::Text("Viewport");

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    GLuint sceneTexture = frameBufferController->GetColorTexture();

    ImGui::Image(
        (ImTextureID)(intptr_t)sceneTexture,
        viewportSize,
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::Separator();

    ImGui::EndChild();

    ImGui::EndChild();

    ImGui::BeginChild("FileBrowser", ImVec2(0, bottomHeight), true);
    ImGui::Text("Asset Browser");
    ImGui::Separator();

    static char searchBuffer[128] = "";
    ImGui::InputText("Search files", searchBuffer, IM_ARRAYSIZE(searchBuffer));

    ImGui::Columns(4, nullptr, false);

    const char* files[] =
    {
        "Assets",
        "Scripts",
        "Materials",
        "Textures",
        "Models",
        "Scenes",
        "Shaders",
        "Audio"
    };

    for (int i = 0; i < IM_ARRAYSIZE(files); ++i)
    {
        ImGui::Button(files[i], ImVec2(120, 60));
        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::EndChild();

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