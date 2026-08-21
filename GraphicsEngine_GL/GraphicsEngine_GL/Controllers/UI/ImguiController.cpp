#include "ImguiController.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Controllers/Window/WindowController.h"
#include "Model/Model.h"
#include "Object/GameObject/GameObject.h"

namespace
{
    struct ModelEditorState
    {
        std::array<char, 256> modelPath{};
        std::array<char, 256> texturePath{};
    };

    std::unordered_map<Model*, ModelEditorState> g_modelEditorStates;

    ModelEditorState& GetModelEditorState(Model* model)
    {
        return g_modelEditorStates[model];
    }

    void RemoveModelEditorState(Model* model)
    {
        if (model != nullptr)
        {
            g_modelEditorStates.erase(model);
        }
    }

    Model* AddModelComponent(GameObject* gameObject)
    {
        if (gameObject == nullptr)
        {
            return nullptr;
        }

        Model* component = gameObject->AddComponent<Model>();
        ModelEditorState& modelEditorState = GetModelEditorState(component);
        modelEditorState.modelPath[0] = '\0';

        return component;
    }

    GameObject* CreateGameObject(
        WorldController* worldController,
        const std::string& baseName,
        const bool addModel)
    {
        if (worldController == nullptr)
        {
            return nullptr;
        }

        auto* gameObject = new GameObject(
            Vector3D(0.0f, 0.0f, 0.0f),
            Vector3D(1.0f, 1.0f, 1.0f));

        const std::size_t objectNumber =
            worldController->GameObjects.size() + 1;

        gameObject->SetName(
            baseName + " " + std::to_string(objectNumber));

        if (addModel)
        {
            gameObject->AddComponent<Model>();
        }

        worldController->AddGameObject(gameObject);

        return gameObject;
    }

    void DestroyGameObject(
        WorldController* worldController,
        GameObject*& selectedGameObject,
        GameObject* gameObject)
    {
        if (worldController == nullptr || gameObject == nullptr)
        {
            return;
        }

        if (Model *model = gameObject->GetComponent<Model>())
        {
            RemoveModelEditorState(model);
        }

        auto& gameObjects = worldController->GameObjects;
        const auto iterator =
            std::find(gameObjects.begin(), gameObjects.end(), gameObject);

        if (iterator == gameObjects.end())
        {
            return;
        }

        if (selectedGameObject == gameObject)
        {
            selectedGameObject = nullptr;
        }

        (*iterator)->Destroy();
        delete *iterator;
        gameObjects.erase(iterator);
    }
}

ImguiController::ImguiController(
    WindowController* controller,
    FrameBufferController* frameBuffer,
    WorldController* controllerWorld)
    : windowController(controller),
      frameBufferController(frameBuffer),
      worldController(controllerWorld),
      window(nullptr),
      selectedGameObject(nullptr)
{
}

ImguiController::~ImguiController() = default;

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
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
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

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    const ImGuiWindowFlags editorFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::Begin("Game Engine Editor", nullptr, editorFlags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
            {
                selectedGameObject = nullptr;
                // worldController->ClearScene();
            }

            if (ImGui::MenuItem("Open Scene"))
            {
                bool LoadedWorld = worldController->LoadWorld("testScene.sav");

                if (!LoadedWorld)
                {
                    std::cerr << "Failed to load scene." << std::endl;
                }
            }

            if (ImGui::MenuItem("Save Scene"))
            {
                bool SavedWorld = worldController->SaveWorld("testScene.sav");

                if (!SavedWorld)
                {
                    std::cerr << "Failed to save scene." << std::endl;
                }
            }

            ImGui::Separator();
            ImGui::MenuItem("Exit");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            if (ImGui::MenuItem("Create Empty"))
            {
                selectedGameObject =
                    CreateGameObject(worldController, "GameObject", false);
            }

            if (ImGui::MenuItem("Create Model"))
            {
                selectedGameObject =
                    CreateGameObject(worldController, "Model", true);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    const ImVec2 available = ImGui::GetContentRegionAvail();
    constexpr float hierarchyWidth = 260.0f;
    constexpr float inspectorWidth = 320.0f;
    constexpr float bottomHeight = 220.0f;

    const float upperHeight =
        available.y > bottomHeight ? available.y - bottomHeight : 0.0f;

    ImGui::BeginChild(
        "UpperArea",
        ImVec2(available.x, upperHeight),
        false);

    // Scene hierarchy
    ImGui::BeginChild(
        "SceneHierarchy",
        ImVec2(hierarchyWidth, 0.0f),
        true);

    ImGui::TextUnformatted("Scene Hierarchy");
    ImGui::Separator();

    if (worldController == nullptr)
    {
        ImGui::TextDisabled("WorldController no disponible.");
    }
    else
    {
        auto& gameObjects = worldController->GameObjects;

        ImGui::Text("GameObjects: %zu", gameObjects.size());
        ImGui::Separator();

        if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (std::size_t i = 0; i < gameObjects.size(); ++i)
            {
                GameObject* gameObject = gameObjects[i];

                if (gameObject == nullptr)
                {
                    continue;
                }

                ImGui::PushID(gameObject);

                const bool isSelected = selectedGameObject == gameObject;
                std::string visibleName = gameObject->GetName();

                if (visibleName.empty())
                {
                    visibleName = "GameObject " + std::to_string(i + 1);
                }

                if (ImGui::Selectable(visibleName.c_str(), isSelected))
                {
                    selectedGameObject = gameObject;
                }

                bool deleteRequested = false;

                if (ImGui::BeginPopupContextItem("GameObjectContextMenu"))
                {
                    const bool alreadyHasModel =
                        gameObject->GetComponent<Model>() != nullptr;

                    if (alreadyHasModel)
                    {
                        ImGui::BeginDisabled();
                    }

                    if (ImGui::MenuItem("Add Model Component"))
                    {
                        AddModelComponent(gameObject);
                    }

                    if (alreadyHasModel)
                    {
                        ImGui::EndDisabled();
                    }

                    ImGui::Separator();
                    deleteRequested = ImGui::MenuItem("Delete");
                    ImGui::EndPopup();
                }

                ImGui::PopID();

                if (deleteRequested)
                {
                    DestroyGameObject(
                        worldController,
                        selectedGameObject,
                        gameObject);
                    break;
                }
            }

            ImGui::TreePop();
        }

        if (ImGui::Button("Create Empty", ImVec2(-1.0f, 0.0f)))
        {
            selectedGameObject =
                CreateGameObject(worldController, "GameObject", false);
        }
    }

    ImGui::EndChild();
    ImGui::SameLine();

    // Viewport
    float viewportWidth =
        ImGui::GetContentRegionAvail().x - inspectorWidth;

    if (viewportWidth < 100.0f)
    {
        viewportWidth = 100.0f;
    }

    const ImGuiWindowFlags viewportFlags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::BeginChild(
        "Viewport",
        ImVec2(viewportWidth, 0.0f),
        true,
        viewportFlags);

    ImGui::TextUnformatted("Viewport");
    ImGui::Separator();

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    viewportSize.x = std::max(viewportSize.x, 1.0f);
    viewportSize.y = std::max(viewportSize.y, 1.0f);

    if (frameBufferController != nullptr)
    {
        const GLuint sceneTexture =
            frameBufferController->GetColorTexture();

        ImGui::Image(
            (ImTextureID)(intptr_t)sceneTexture,
            viewportSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );
    }
    else
    {
        ImGui::TextDisabled("FrameBufferController no disponible.");
    }

    ImGui::EndChild();
    ImGui::SameLine();

    // Inspector
    ImGui::BeginChild("Inspector", ImVec2(0.0f, 0.0f), true);

    ImGui::TextUnformatted("Inspector");
    ImGui::Separator();

    if (selectedGameObject == nullptr)
    {
        ImGui::TextDisabled("No hi ha cap GameObject seleccionat.");
    }
    else
    {
        char nameBuffer[128]{};
        const std::string& currentName = selectedGameObject->GetName();

        std::snprintf(
            nameBuffer,
            sizeof(nameBuffer),
            "%s",
            currentName.c_str());

        ImGui::SetNextItemWidth(-1.0f);

        if (ImGui::InputText(
                "##GameObjectName",
                nameBuffer,
                sizeof(nameBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue) ||
            ImGui::IsItemDeactivatedAfterEdit())
        {
            selectedGameObject->SetName(nameBuffer);
        }

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::CollapsingHeader(
                "Transform",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            float position[3] = {
                selectedGameObject->position.x,
                selectedGameObject->position.y,
                selectedGameObject->position.z};

            float rotation[3] = {
                selectedGameObject->rotation.x,
                selectedGameObject->rotation.y,
                selectedGameObject->rotation.z};

            float scale[3] = {
                selectedGameObject->size.x,
                selectedGameObject->size.y,
                selectedGameObject->size.z};

            if (ImGui::DragFloat3("Position", position, 0.1f))
            {
                selectedGameObject->position =
                    Vector3D(position[0], position[1], position[2]);
            }

            if (ImGui::DragFloat3("Rotation", rotation, 0.5f))
            {
                selectedGameObject->rotation =
                    Vector3D(rotation[0], rotation[1], rotation[2]);
            }

            if (ImGui::DragFloat3(
                    "Scale",
                    scale,
                    0.05f,
                    0.001f,
                    1000.0f))
            {
                selectedGameObject->size =
                    Vector3D(scale[0], scale[1], scale[2]);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextUnformatted("Components");
        ImGui::Spacing();

        const auto& components =
            selectedGameObject->GetFullComponentsList();

        if (components.empty())
        {
            ImGui::TextDisabled("Aquest GameObject no té components.");
        }

        for (std::size_t i = 0; i < components.size(); ++i)
        {
            Component* component = components[i];

            if (component == nullptr)
            {
                continue;
            }

            ImGui::PushID(component);

            if (Model* modelComponent = dynamic_cast<Model*>(component))
            {
                if (ImGui::CollapsingHeader(
                        "Model",
                        ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Text(
                        "Vertices: %u",
                        modelComponent->GetTotalVertices());
                    ImGui::Text(
                        "Triangles: %u",
                        modelComponent->GetTotalTriangles());

                    float componentScale[3] = {
                        selectedGameObject->size.x,
                        selectedGameObject->size.y,
                        selectedGameObject->size.z};

                    if (ImGui::DragFloat3(
                            "Model Scale",
                            componentScale,
                            0.05f,
                            0.001f,
                            1000.0f))
                    {
                        selectedGameObject->size = Vector3D(
                            componentScale[0],
                            componentScale[1],
                            componentScale[2]);
                    }

                    ModelEditorState& editorState =
                        GetModelEditorState(modelComponent);

                    ImGui::InputText(
                        "Model file",
                        editorState.modelPath.data(),
                        editorState.modelPath.size());

                    ImGui::InputText(
                        "Texture file",
                        editorState.texturePath.data(),
                        editorState.texturePath.size());

                    const bool hasModelPath =
                        editorState.modelPath[0] != '\0';

                    if (!hasModelPath)
                    {
                        ImGui::BeginDisabled();
                    }

                    if (ImGui::Button("Load Model"))
                    {
                        try
                        {
                            if (editorState.texturePath[0] != '\0')
                            {
                                modelComponent->LoadModel(
                                    editorState.modelPath.data(),
                                    editorState.texturePath.data());
                            }
                            else
                            {
                                modelComponent->LoadModel(
                                    editorState.modelPath.data());
                            }

                            modelComponent->CalcNumVerticesTriangles();
                        }
                        catch (const std::exception& exception)
                        {
                            std::cerr
                                << "Error loading model: "
                                << exception.what()
                                << '\n';
                        }
                    }

                    if (!hasModelPath)
                    {
                        ImGui::EndDisabled();
                        ImGui::TextDisabled(
                            "Selecciona un fitxer de model abans de carregar.");
                    }
                }
            }
            else
            {
                ImGui::TextDisabled("Component desconegut");
            }

            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Add Component", ImVec2(-1.0f, 0.0f)))
        {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup"))
        {
            const bool alreadyHasModel =
                selectedGameObject->GetComponent<Model>() != nullptr;

            if (alreadyHasModel)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::MenuItem("Model"))
            {
                AddModelComponent(selectedGameObject);
            }

            if (alreadyHasModel)
            {
                ImGui::EndDisabled();
            }

            ImGui::EndPopup();
        }

        ImGui::Spacing();

        if (ImGui::Button(
                "Delete GameObject",
                ImVec2(-1.0f, 0.0f)))
        {
            ImGui::OpenPopup("DeleteGameObjectPopup");
        }

        if (ImGui::BeginPopupModal(
                "DeleteGameObjectPopup",
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(
                "Vols eliminar '%s'?",
                selectedGameObject->GetName().c_str());

            ImGui::Spacing();

            if (ImGui::Button("Delete", ImVec2(120.0f, 0.0f)))
            {
                GameObject* objectToDelete = selectedGameObject;
                DestroyGameObject(
                    worldController,
                    selectedGameObject,
                    objectToDelete);
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    ImGui::EndChild();
    ImGui::EndChild();

    // Asset browser
    ImGui::BeginChild(
        "FileBrowser",
        ImVec2(0.0f, bottomHeight),
        true);

    ImGui::TextUnformatted("Asset Browser");
    ImGui::Separator();

    static char searchBuffer[128] = "";
    ImGui::SetNextItemWidth(300.0f);
    ImGui::InputTextWithHint(
        "##AssetSearch",
        "Search files...",
        searchBuffer,
        IM_ARRAYSIZE(searchBuffer));

    ImGui::Spacing();

    const char* files[] = {
        "Assets",
        "Scripts",
        "Materials",
        "Textures",
        "Models",
        "Scenes",
        "Shaders",
        "Audio"};

    constexpr float assetButtonWidth = 120.0f;
    constexpr float assetButtonHeight = 60.0f;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float contentWidth = ImGui::GetContentRegionAvail().x;

    int columnCount = static_cast<int>(
        contentWidth / (assetButtonWidth + spacing));
    columnCount = std::max(columnCount, 1);

    ImGui::Columns(columnCount, "AssetBrowserColumns", false);

    for (const char* file : files)
    {
        ImGui::Button(
            file,
            ImVec2(assetButtonWidth, assetButtonHeight));
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
    g_modelEditorStates.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window = nullptr;
    selectedGameObject = nullptr;

    std::cout << "ImguiController: CleanUp OK\n";
    return true;
}