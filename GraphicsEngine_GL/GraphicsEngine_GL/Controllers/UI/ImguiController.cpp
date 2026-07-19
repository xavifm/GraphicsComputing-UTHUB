#include "ImguiController.h"

#include <iostream>
#include "Controllers/Window/WindowController.h"
#include "Object/GameObject/GameObject.h"
#include "Model/Model.h"
#include <algorithm>

ImguiController::ImguiController(WindowController* controller, FrameBufferController* frameBuffer, WorldController* controllerWorld)
{
    windowController = controller;
    frameBufferController = frameBuffer;
    worldController = controllerWorld;
    selectedGameObject = nullptr;
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

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(io.DisplaySize);

    const ImGuiWindowFlags editorFlags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::Begin(
        "Game Engine Editor",
        nullptr,
        editorFlags
    );

    // ---------------------------------------------------------------------
    // Barra de menú
    // ---------------------------------------------------------------------

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
            {
                selectedGameObject = nullptr;

                // Cal implementar la neteja des de WorldController.
                // worldController->ClearScene();
            }

            if (ImGui::MenuItem("Open Scene"))
            {
                // Obrir escena.
            }

            if (ImGui::MenuItem("Save Scene"))
            {
                // Guardar escena.
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
                // Tancar aplicació.
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject"))
        {
            if (ImGui::MenuItem("Create Empty"))
            {
                if (worldController != nullptr)
                {
                    GameObject* newGameObject = new GameObject(
                        Vector3D(0.0f, 0.0f, 0.0f),
                        Vector3D(1.0f, 1.0f, 1.0f)
                    );

                    const std::size_t objectNumber =
                        worldController->GameObjects.size() + 1;

                    newGameObject->SetName(
                        "GameObject " + std::to_string(objectNumber)
                    );

                    worldController->GameObjects.push_back(newGameObject);

                    newGameObject->Start();

                    selectedGameObject = newGameObject;
                }
            }

            if (ImGui::MenuItem("Create Model"))
            {
                if (worldController != nullptr)
                {
                    GameObject* newGameObject = new GameObject(
                        Vector3D(0.0f, 0.0f, 0.0f),
                        Vector3D(1.0f, 1.0f, 1.0f)
                    );

                    const std::size_t objectNumber =
                        worldController->GameObjects.size() + 1;

                    newGameObject->SetName(
                        "Model " + std::to_string(objectNumber)
                    );

                    newGameObject->AddComponent(
                        std::make_unique<Model>()
                    );

                    worldController->GameObjects.push_back(newGameObject);

                    newGameObject->Start();

                    selectedGameObject = newGameObject;
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImVec2 available = ImGui::GetContentRegionAvail();

    const float hierarchyWidth = 260.0f;
    const float inspectorWidth = 320.0f;
    const float bottomHeight = 220.0f;

    const float upperHeight =
        available.y > bottomHeight
            ? available.y - bottomHeight
            : 0.0f;

    // ---------------------------------------------------------------------
    // Zona superior
    // ---------------------------------------------------------------------

    ImGui::BeginChild(
        "UpperArea",
        ImVec2(available.x, upperHeight),
        false
    );

    // ---------------------------------------------------------------------
    // Scene Hierarchy
    // ---------------------------------------------------------------------

    ImGui::BeginChild(
        "SceneHierarchy",
        ImVec2(hierarchyWidth, 0.0f),
        true
    );

    ImGui::TextUnformatted("Scene Hierarchy");
    ImGui::Separator();

    if (worldController == nullptr)
    {
        ImGui::TextDisabled("WorldController no disponible.");
    }
    else
    {
        std::vector<GameObject*>& gameObjects =
            worldController->GameObjects;

        ImGui::Text(
            "GameObjects: %zu",
            gameObjects.size()
        );

        ImGui::Separator();

        if (ImGui::TreeNodeEx(
                "Scene",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (std::size_t i = 0; i < gameObjects.size(); ++i)
            {
                GameObject* gameObject = gameObjects[i];

                if (gameObject == nullptr)
                {
                    continue;
                }

                ImGui::PushID(static_cast<int>(i));

                const bool isSelected =
                    selectedGameObject == gameObject;

                std::string visibleName = gameObject->GetName();

                if (visibleName.empty())
                {
                    visibleName =
                        "GameObject " + std::to_string(i);
                }

                if (ImGui::Selectable(
                        visibleName.c_str(),
                        isSelected))
                {
                    selectedGameObject = gameObject;
                }

                if (ImGui::BeginPopupContextItem(
                        "GameObjectContextMenu"))
                {
                    if (ImGui::MenuItem("Add Model Component"))
                    {
                        if (gameObject->GetComponent<Model>() == nullptr)
                        {
                            gameObject->AddComponent(
                                std::make_unique<Model>()
                            );
                        }
                    }

                    ImGui::Separator();

                    if (ImGui::MenuItem("Delete"))
                    {
                        if (selectedGameObject == gameObject)
                        {
                            selectedGameObject = nullptr;
                        }

                        gameObject->Destroy();
                        delete gameObject;

                        gameObjects.erase(
                            gameObjects.begin() +
                            static_cast<std::ptrdiff_t>(i)
                        );

                        ImGui::EndPopup();
                        ImGui::PopID();

                        break;
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();
            }

            ImGui::TreePop();
        }

        if (ImGui::Button(
                "Create Empty",
                ImVec2(-1.0f, 0.0f)))
        {
            GameObject* newGameObject = new GameObject(
                Vector3D(0.0f, 0.0f, 0.0f),
                Vector3D(1.0f, 1.0f, 1.0f)
            );

            const std::size_t objectNumber =
                gameObjects.size() + 1;

            newGameObject->SetName(
                "GameObject " + std::to_string(objectNumber)
            );

            gameObjects.push_back(newGameObject);

            newGameObject->Start();

            selectedGameObject = newGameObject;
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ---------------------------------------------------------------------
    // Viewport
    // ---------------------------------------------------------------------

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
        viewportFlags
    );

    ImGui::TextUnformatted("Viewport");
    ImGui::Separator();

    ImVec2 viewportSize =
        ImGui::GetContentRegionAvail();

    if (viewportSize.x < 1.0f)
    {
        viewportSize.x = 1.0f;
    }

    if (viewportSize.y < 1.0f)
    {
        viewportSize.y = 1.0f;
    }

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
        ImGui::TextDisabled(
            "FrameBufferController no disponible."
        );
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ---------------------------------------------------------------------
    // Inspector
    // ---------------------------------------------------------------------

    ImGui::BeginChild(
        "Inspector",
        ImVec2(0.0f, 0.0f),
        true
    );

    ImGui::TextUnformatted("Inspector");
    ImGui::Separator();

    if (selectedGameObject == nullptr)
    {
        ImGui::TextDisabled(
            "No hi ha cap GameObject seleccionat."
        );
    }
    else
    {
        // -----------------------------------------------------------------
        // Nom
        // -----------------------------------------------------------------

        char nameBuffer[128] = {};

        const std::string& currentName =
            selectedGameObject->GetName();

        std::snprintf(
            nameBuffer,
            sizeof(nameBuffer),
            "%s",
            currentName.c_str()
        );

        ImGui::SetNextItemWidth(-1.0f);

        if (ImGui::InputText(
                "##GameObjectName",
                nameBuffer,
                sizeof(nameBuffer),
                ImGuiInputTextFlags_EnterReturnsTrue))
        {
            selectedGameObject->SetName(nameBuffer);
        }

        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            selectedGameObject->SetName(nameBuffer);
        }

        ImGui::Spacing();
        ImGui::Separator();

        // -----------------------------------------------------------------
        // Transform
        // -----------------------------------------------------------------

        if (ImGui::CollapsingHeader(
                "Transform",
                ImGuiTreeNodeFlags_DefaultOpen))
        {
            float position[3] =
            {
                selectedGameObject->position.x,
                selectedGameObject->position.y,
                selectedGameObject->position.z
            };

            float rotation[3] =
            {
                selectedGameObject->rotation.x,
                selectedGameObject->rotation.y,
                selectedGameObject->rotation.z
            };

            float scale[3] =
            {
                selectedGameObject->size.x,
                selectedGameObject->size.y,
                selectedGameObject->size.z
            };

            if (ImGui::DragFloat3(
                    "Position",
                    position,
                    0.1f))
            {
                selectedGameObject->position =
                    Vector3D(
                        position[0],
                        position[1],
                        position[2]
                    );
            }

            if (ImGui::DragFloat3(
                    "Rotation",
                    rotation,
                    0.5f))
            {
                selectedGameObject->rotation =
                    Vector3D(
                        rotation[0],
                        rotation[1],
                        rotation[2]
                    );
            }

            if (ImGui::DragFloat3(
                    "Scale",
                    scale,
                    0.05f,
                    0.001f,
                    1000.0f))
            {
                selectedGameObject->size =
                    Vector3D(
                        scale[0],
                        scale[1],
                        scale[2]
                    );
            }
        }

        // -----------------------------------------------------------------
        // Components
        // -----------------------------------------------------------------

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextUnformatted("Components");
        ImGui::Spacing();

        const auto& components =
            selectedGameObject->GetFullComponentsList();

        if (components.empty())
        {
            ImGui::TextDisabled(
                "Aquest GameObject no té components."
            );
        }

        for (std::size_t i = 0;
             i < components.size();
             ++i)
        {
            Component* component =
                components[i].get();

            if (component == nullptr)
            {
                continue;
            }

            ImGui::PushID(static_cast<int>(i));

            Model* modelComponent =
                dynamic_cast<Model*>(component);

            if (modelComponent != nullptr)
            {
                const bool modelOpen =
                    ImGui::CollapsingHeader(
                        "Model",
                        ImGuiTreeNodeFlags_DefaultOpen
                    );

                if (modelOpen)
                {
                    ImGui::Text(
                        "Vertices: %u",
                        modelComponent->GetTotalVertices()
                    );

                    ImGui::Text(
                        "Triangles: %u",
                        modelComponent->GetTotalTriangles()
                    );

                    Vector3D modelPosition =
                        selectedGameObject->position;

                    float componentPosition[3] =
                    {
                        modelPosition.x,
                        modelPosition.y,
                        modelPosition.z
                    };

                    Vector3D modelScale =
                        selectedGameObject->size;

                    float componentScale[3] =
                    {
                        modelScale.x,
                        modelScale.y,
                        modelScale.z
                    };

                    if (ImGui::DragFloat3(
                            "Model Scale",
                            componentScale,
                            0.05f,
                            0.001f,
                            1000.0f))
                    {
                        selectedGameObject->size =
                            Vector3D(
                                componentScale[0],
                                componentScale[1],
                                componentScale[2]);
                    }

                    static char modelPath[256] = "";
                    static char texturePath[256] = "";

                    ImGui::InputText(
                        "Model file",
                        modelPath,
                        IM_ARRAYSIZE(modelPath)
                    );

                    ImGui::InputText(
                        "Texture file",
                        texturePath,
                        IM_ARRAYSIZE(texturePath)
                    );

                    if (ImGui::Button("Load Model"))
                    {
                        if (texturePath[0] != '\0')
                        {
                            modelComponent->LoadModel(
                                modelPath,
                                texturePath
                            );
                        }
                        else
                        {
                            modelComponent->LoadModel(
                                modelPath
                            );
                        }

                        modelComponent
                            ->CalcNumVerticesTriangles();
                    }
                }
            }
            else
            {
                ImGui::TextDisabled(
                    "Component desconegut"
                );
            }

            ImGui::PopID();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // -----------------------------------------------------------------
        // Afegir components
        // -----------------------------------------------------------------

        if (ImGui::Button(
                "Add Component",
                ImVec2(-1.0f, 0.0f)))
        {
            ImGui::OpenPopup(
                "AddComponentPopup"
            );
        }

        if (ImGui::BeginPopup(
                "AddComponentPopup"))
        {
            const bool alreadyHasModel =
                selectedGameObject
                    ->GetComponent<Model>() != nullptr;

            if (alreadyHasModel)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::MenuItem("Model"))
            {
                selectedGameObject->AddComponent(
                    std::make_unique<Model>()
                );
            }

            if (alreadyHasModel)
            {
                ImGui::EndDisabled();
            }

            ImGui::EndPopup();
        }

        ImGui::Spacing();

        // -----------------------------------------------------------------
        // Eliminar GameObject
        // -----------------------------------------------------------------

        if (ImGui::Button(
                "Delete GameObject",
                ImVec2(-1.0f, 0.0f)))
        {
            ImGui::OpenPopup(
                "DeleteGameObjectPopup"
            );
        }

        if (ImGui::BeginPopupModal(
                "DeleteGameObjectPopup",
                nullptr,
                ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(
                "Vols eliminar '%s'?",
                selectedGameObject
                    ->GetName()
                    .c_str()
            );

            ImGui::Spacing();

            if (ImGui::Button(
                    "Delete",
                    ImVec2(120.0f, 0.0f)))
            {
                if (worldController != nullptr)
                {
                    std::vector<GameObject*>& gameObjects =
                        worldController->GameObjects;

                    const auto iterator =
                        std::find(
                            gameObjects.begin(),
                            gameObjects.end(),
                            selectedGameObject
                        );

                    if (iterator != gameObjects.end())
                    {
                        (*iterator)->Destroy();
                        delete *iterator;

                        gameObjects.erase(iterator);
                    }
                }

                selectedGameObject = nullptr;

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button(
                    "Cancel",
                    ImVec2(120.0f, 0.0f)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    ImGui::EndChild();

    ImGui::EndChild();

    // ---------------------------------------------------------------------
    // Asset Browser
    // ---------------------------------------------------------------------

    ImGui::BeginChild(
        "FileBrowser",
        ImVec2(0.0f, bottomHeight),
        true
    );

    ImGui::TextUnformatted("Asset Browser");
    ImGui::Separator();

    static char searchBuffer[128] = "";

    ImGui::SetNextItemWidth(300.0f);

    ImGui::InputTextWithHint(
        "##AssetSearch",
        "Search files...",
        searchBuffer,
        IM_ARRAYSIZE(searchBuffer)
    );

    ImGui::Spacing();

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

    const float assetButtonWidth = 120.0f;
    const float assetButtonHeight = 60.0f;
    const float spacing =
        ImGui::GetStyle().ItemSpacing.x;

    const float contentWidth =
        ImGui::GetContentRegionAvail().x;

    int columnCount =
        static_cast<int>(
            contentWidth /
            (assetButtonWidth + spacing)
        );

    if (columnCount < 1)
    {
        columnCount = 1;
    }

    ImGui::Columns(
        columnCount,
        "AssetBrowserColumns",
        false
    );

    for (int i = 0;
         i < IM_ARRAYSIZE(files);
         ++i)
    {
        if (ImGui::Button(
                files[i],
                ImVec2(
                    assetButtonWidth,
                    assetButtonHeight
                )))
        {
            // Obrir carpeta.
        }

        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::EndChild();

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );

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