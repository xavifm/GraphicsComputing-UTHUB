#include "ImguiController.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Component/ComponentRegistry.h"
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

        for (GameObject* sceneObject : gameObjects)
        {
            if (sceneObject != nullptr && sceneObject->parent == gameObject)
            {
                sceneObject->parent = nullptr;
            }
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

        // Petició de reparentatge diferida fins que s'hagi acabat
        // de dibuixar la jerarquia del frame actual.
        GameObject* objectToReparent = nullptr;
        GameObject* newParent = nullptr;
        bool reparentRequested = false;

        const bool sceneOpen =
            ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_DefaultOpen);

        // Drop sobre Scene = convertir l'objecte en root.
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload =
                    ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY"))
            {
                if (payload->DataSize == sizeof(GameObject*))
                {
                    GameObject* draggedObject =
                        *static_cast<GameObject* const*>(payload->Data);

                    if (draggedObject != nullptr)
                    {
                        objectToReparent = draggedObject;
                        newParent = nullptr;
                        reparentRequested = true;
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (sceneOpen)
        {
            GameObject* gameObjectToDelete = nullptr;

            auto& sceneObjects = gameObjects;

            auto existsInScene = [&](GameObject* object) -> bool
            {
                return object != nullptr &&
                       std::find(
                           sceneObjects.begin(),
                           sceneObjects.end(),
                           object) != sceneObjects.end();
            };

            std::unordered_map<GameObject*, std::vector<GameObject*>> children;
            std::vector<GameObject*> roots;

            for (GameObject* gameObject : sceneObjects)
            {
                if (gameObject == nullptr)
                {
                    continue;
                }

                if (gameObject->parent != nullptr &&
                    gameObject->parent != gameObject &&
                    existsInScene(gameObject->parent))
                {
                    children[gameObject->parent].push_back(gameObject);
                }
                else
                {
                    roots.push_back(gameObject);
                }
            }

            std::unordered_set<GameObject*> drawnObjects;

            auto drawGameObject =
                [&](auto&& self, GameObject* gameObject) -> void
                {
                    if (gameObject == nullptr)
                    {
                        return;
                    }

                    if (!drawnObjects.insert(gameObject).second)
                    {
                        return;
                    }

                    const auto childrenIt = children.find(gameObject);
                    const bool hasChildren =
                        childrenIt != children.end() &&
                        !childrenIt->second.empty();

                    std::string visibleName = gameObject->GetName();

                    if (visibleName.empty())
                    {
                        visibleName = "GameObject";
                    }

                    ImGui::PushID(gameObject);

                    ImGuiTreeNodeFlags flags =
                        ImGuiTreeNodeFlags_OpenOnArrow |
                        ImGuiTreeNodeFlags_OpenOnDoubleClick |
                        ImGuiTreeNodeFlags_SpanAvailWidth;

                    if (selectedGameObject == gameObject)
                    {
                        flags |= ImGuiTreeNodeFlags_Selected;
                    }

                    if (hasChildren)
                    {
                        flags |= ImGuiTreeNodeFlags_DefaultOpen;
                    }
                    else
                    {
                        flags |= ImGuiTreeNodeFlags_Leaf |
                                 ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    }

                    const bool nodeOpen = ImGui::TreeNodeEx(
                        "##GameObjectHierarchyNode",
                        flags,
                        "%s",
                        visibleName.c_str());

                    // Drag source: permet arrossegar aquest GameObject.
                    if (ImGui::BeginDragDropSource())
                    {
                        GameObject* draggedObject = gameObject;

                        ImGui::SetDragDropPayload(
                            "GAMEOBJECT_HIERARCHY",
                            &draggedObject,
                            sizeof(GameObject*));

                        ImGui::Text(
                            "Move %s",
                            visibleName.c_str());

                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload =
                                ImGui::AcceptDragDropPayload(
                                    "GAMEOBJECT_HIERARCHY"))
                        {
                            if (payload->DataSize == sizeof(GameObject*))
                            {
                                GameObject* draggedObject =
                                    *static_cast<GameObject* const*>(
                                        payload->Data);

                                if (draggedObject != nullptr &&
                                    draggedObject != gameObject)
                                {
                                    bool createsCycle = false;
                                    GameObject* ancestor = gameObject;

                                    while (ancestor != nullptr)
                                    {
                                        if (ancestor == draggedObject)
                                        {
                                            createsCycle = true;
                                            break;
                                        }

                                        ancestor = ancestor->parent;
                                    }

                                    if (!createsCycle)
                                    {
                                        objectToReparent = draggedObject;
                                        newParent = gameObject;
                                        reparentRequested = true;
                                    }
                                }
                            }
                        }

                        ImGui::EndDragDropTarget();
                    }

                    if (ImGui::IsItemClicked() &&
                        !ImGui::IsItemToggledOpen())
                    {
                        selectedGameObject = gameObject;
                    }

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

                        if (ImGui::MenuItem("Delete"))
                        {
                            gameObjectToDelete = gameObject;
                        }

                        ImGui::EndPopup();
                    }

                    if (hasChildren && nodeOpen)
                    {
                        for (GameObject* child : childrenIt->second)
                        {
                            self(self, child);
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                };

            // Primer dibuixem totes les arrels.
            for (GameObject* root : roots)
            {
                drawGameObject(drawGameObject, root);
            }

            // Fallback de seguretat:
            // si hi ha dades de parentatge circulars/corruptes, cap objecte
            // de GameObjects queda completament invisible.
            for (GameObject* gameObject : sceneObjects)
            {
                if (gameObject != nullptr &&
                    drawnObjects.find(gameObject) == drawnObjects.end())
                {
                    drawGameObject(drawGameObject, gameObject);
                }
            }

            if (gameObjectToDelete != nullptr)
            {
                DestroyGameObject(
                    worldController,
                    selectedGameObject,
                    gameObjectToDelete);
            }

            ImGui::TreePop();
        }

        // Apliquem el canvi quan ja no estem recorrent la jerarquia.
        if (reparentRequested && objectToReparent != nullptr)
        {
            objectToReparent->parent = newParent;
            selectedGameObject = objectToReparent;
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

        ImGuiFieldVisitor visitor;

        for (std::size_t i = 0; i < components.size(); ++i)
        {
            Component* component = components[i];

            if (component == nullptr)
            {
                continue;
            }

            ImGui::PushID(component);

            if (ImGui::CollapsingHeader(
            component->GetTypeName(),
            ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (!ComponentRegistry::VisitFields(*component, visitor))
                {
                    ImGui::TextDisabled("Component no registrat");
                }
            }

            //INTERNAL ENGINE FUNCTIONS ONLY
            if (Model* modelComponent = dynamic_cast<Model*>(component))
            {
                if (ImGui::Button("Load Model"))
                {
                    try
                    {
                        ModelEditorState& editorState = GetModelEditorState(modelComponent);

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

    DrawAssetBrowser();

    ImGui::EndChild();
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return UPDATE_CONTINUE;
}

void ImguiController::DrawAssetBrowser()
{
    static fs::path rootDir;
    static fs::path currentDir;
    static fs::path selectedEntry;
    static fs::path pendingDelete;

    static char folderName[256] = "";
    static char componentName[256] = "";

    static std::string errorMessage;
    static bool initialized = false;

    // Initialize the Assets directory
    if (!initialized)
    {
        std::error_code ec;

        fs::create_directories("./Assets", ec);

        if (!ec)
            rootDir = fs::canonical("./Assets", ec);

        if (ec)
        {
            ImGui::Text("Error: %s", ec.message().c_str());
            return;
        }

        currentDir = rootDir;
        initialized = true;
    }

    // Check whether a path is inside the Assets directory
    auto isInsideAssets = [&](const fs::path& path) -> bool
    {
        std::error_code ec;
        fs::path resolved = fs::weakly_canonical(path, ec);

        if (ec)
            return false;

        auto rootIt = rootDir.begin();
        auto pathIt = resolved.begin();

        for (; rootIt != rootDir.end(); ++rootIt, ++pathIt)
        {
            if (pathIt == resolved.end() || *rootIt != *pathIt)
                return false;
        }

        return true;
    };

    // Validate a C++ class identifier
    auto isValidClassName = [](const std::string& name) -> bool
    {
        if (name.empty())
            return false;

        unsigned char first = static_cast<unsigned char>(name[0]);

        if (!std::isalpha(first) && first != '_')
            return false;

        for (unsigned char c : name)
        {
            if (!std::isalnum(c) && c != '_')
                return false;
        }

        // Reject reserved C++ keywords
        static const std::vector<std::string> keywords = {
            "alignas", "alignof", "and", "and_eq", "asm",
            "auto", "bitand", "bitor", "bool", "break",
            "case", "catch", "char", "char8_t", "char16_t",
            "char32_t", "class", "compl", "concept", "const",
            "consteval", "constexpr", "constinit", "const_cast",
            "continue", "co_await", "co_return", "co_yield",
            "decltype", "default", "delete", "do", "double",
            "dynamic_cast", "else", "enum", "explicit",
            "export", "extern", "false", "float", "for",
            "friend", "goto", "if", "inline", "int", "long",
            "mutable", "namespace", "new", "noexcept", "not",
            "not_eq", "nullptr", "operator", "or", "or_eq",
            "private", "protected", "public", "register",
            "reinterpret_cast", "requires", "return", "short",
            "signed", "sizeof", "static", "static_assert",
            "static_cast", "struct", "switch", "template",
            "this", "thread_local", "throw", "true", "try",
            "typedef", "typeid", "typename", "union",
            "unsigned", "using", "virtual", "void",
            "volatile", "wchar_t", "while", "xor", "xor_eq"
        };

        if (std::find(keywords.begin(), keywords.end(), name)
            != keywords.end())
        {
            return false;
        }

        // Reject identifiers reserved for the implementation
        if (name.find("__") != std::string::npos)
            return false;

        if (name.size() >= 2 &&
            name[0] == '_' &&
            std::isupper(static_cast<unsigned char>(name[1])))
        {
            return false;
        }

        return true;
    };

    // Keep navigation restricted to Assets
    if (!isInsideAssets(currentDir))
        currentDir = rootDir;

    // Navigation toolbar
    if (ImGui::Button("<"))
    {
        if (currentDir != rootDir)
        {
            fs::path parent = currentDir.parent_path();

            if (isInsideAssets(parent))
            {
                currentDir = parent;
                selectedEntry.clear();
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Assets"))
    {
        currentDir = rootDir;
        selectedEntry.clear();
    }

    ImGui::SameLine();

    fs::path relativePath = currentDir.lexically_relative(rootDir);

    std::string displayPath =
        relativePath.empty() || relativePath == "."
        ? "/"
        : "/ " + relativePath.generic_string();

    ImGui::TextUnformatted(displayPath.c_str());

    // Create folder button
    if (ImGui::Button("+ Folder"))
    {
        folderName[0] = '\0';
        ImGui::OpenPopup("CreateFolder");
    }

    ImGui::SameLine();

    // Create C++ component button
    if (ImGui::Button("+ Component"))
    {
        componentName[0] = '\0';
        ImGui::OpenPopup("CreateComponent");
    }

    ImGui::Separator();

    // Create folder dialog
    if (ImGui::BeginPopupModal(
        "CreateFolder",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Folder Name:");

        ImGui::InputText(
            "##FolderName",
            folderName,
            sizeof(folderName));

        if (ImGui::Button("Create"))
        {
            std::string name = folderName;

            bool validName =
                !name.empty() &&
                name != "." &&
                name != ".." &&
                name.find('/') == std::string::npos &&
                name.find('\\') == std::string::npos;

            if (validName)
            {
                fs::path newPath = currentDir / name;

                if (isInsideAssets(newPath))
                {
                    std::error_code ec;

                    bool created = fs::create_directory(newPath, ec);

                    if (created)
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    else
                    {
                        errorMessage = ec
                            ? ec.message()
                            : "Folder already exists.";
                    }
                }
                else
                {
                    errorMessage = "Path is outside Assets.";
                }
            }
            else
            {
                errorMessage = "Invalid folder name.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    // Create component from ExampleComponent.h (in the current Asset Browser folder)
    if (ImGui::BeginPopupModal(
        "CreateComponent", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Component Class Name:");
        ImGui::InputText("##ComponentName", componentName, sizeof(componentName));
        ImGui::TextWrapped("Destination: %s", currentDir.string().c_str());

        if (ImGui::Button("Create Component"))
        {
            const std::string name = componentName;
            if (!isValidClassName(name))
            {
                errorMessage = "Invalid C++ class name.";
            }
            else if (!isInsideAssets(currentDir) ||
                     !fs::is_directory(currentDir))
            {
                errorMessage = "Invalid destination folder.";
            }
            else
            {
                // Locate the real header template. Do not generate a hard-coded class.
                const fs::path projectRoot = rootDir.parent_path();
                const std::vector<fs::path> templateCandidates = {
                    projectRoot / "GraphicsEngine_GL" / "Component" / "ExampleComponent.h",
                    projectRoot / "Component" / "ExampleComponent.h",
                    projectRoot.parent_path() / "GraphicsEngine_GL" / "Component" / "ExampleComponent.h",
                    projectRoot / "ExampleComponent.h",
                    rootDir / "ExampleComponent.h"
                };

                fs::path templateHeader;
                for (const fs::path& candidate : templateCandidates)
                {
                    std::error_code checkEc;
                    if (fs::is_regular_file(candidate, checkEc) && !checkEc)
                    {
                        templateHeader = candidate;
                        break;
                    }
                }

                if (templateHeader.empty())
                {
                    errorMessage = "ExampleComponent.h template not found.";
                }
                else
                {
                    const fs::path headerPath = currentDir / (name + ".h");
                    const fs::path templateCpp =
                        templateHeader.parent_path() / "ExampleComponent.cpp";
                    std::error_code templateEc;
                    const bool hasCppTemplate =
                        fs::is_regular_file(templateCpp, templateEc) && !templateEc;
                    const fs::path sourcePath = currentDir / (name + ".cpp");
                    std::error_code ec;

                    const bool headerExists = fs::exists(headerPath, ec);
                    if (ec)
                        errorMessage = ec.message();
                    else
                    {
                        const bool sourceExists = fs::exists(sourcePath, ec);
                        if (ec)
                            errorMessage = ec.message();
                        else if (headerExists || (hasCppTemplate && sourceExists))
                            errorMessage = "Component files already exist.";
                        else if (!isInsideAssets(headerPath) ||
                                 (hasCppTemplate && !isInsideAssets(sourcePath)))
                            errorMessage = "Destination is outside Assets.";
                        else
                        {
                            // Read the template exactly as stored on disk.
                            auto readFile = [](const fs::path& path,
                                               std::string& content) -> bool
                            {
                                std::ifstream file(path, std::ios::binary);
                                if (!file)
                                    return false;
                                content.assign(
                                    std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>());
                                return !file.bad();
                            };

                            auto replaceAll = [](std::string& content,
                                                 const std::string& from,
                                                 const std::string& to)
                            {
                                if (from.empty()) return;
                                std::size_t pos = 0;
                                while ((pos = content.find(from, pos)) !=
                                       std::string::npos)
                                {
                                    content.replace(pos, from.size(), to);
                                    pos += to.size();
                                }
                            };

                            auto uppercase = [](std::string text)
                            {
                                for (char& ch : text)
                                    ch = static_cast<char>(std::toupper(
                                        static_cast<unsigned char>(ch)));
                                return text;
                            };

                            auto adaptClassName = [&](std::string& content)
                            {
                                // Preserve the template's layout, fields,
                                // methods, inheritance and namespaces.
                                replaceAll(content, "EXAMPLE_COMPONENT", uppercase(name));
                                replaceAll(content, "EXAMPLECOMPONENT", uppercase(name));
                                replaceAll(content, "ExampleComponent", name);
                            };

                            std::string headerContent;
                            std::string cppContent;
                            if (!readFile(templateHeader, headerContent))
                                errorMessage = "Could not read ExampleComponent.h.";
                            else if (headerContent.find("ExampleComponent") ==
                                     std::string::npos)
                                errorMessage = "Template does not contain ExampleComponent.";
                            else if (hasCppTemplate &&
                                     !readFile(templateCpp, cppContent))
                                errorMessage = "Could not read ExampleComponent.cpp.";
                            else
                            {
                                adaptClassName(headerContent);
                                if (hasCppTemplate)
                                    adaptClassName(cppContent);

                                // Roll back only files created by this operation.
                                auto writeFile = [](const fs::path& path,
                                                    const std::string& content) -> bool
                                {
                                    std::ofstream file(path, std::ios::binary | std::ios::out);
                                    if (!file) return false;
                                    file.write(content.data(),
                                               static_cast<std::streamsize>(content.size()));
                                    file.close();
                                    return static_cast<bool>(file);
                                };

                                // Check was done above; don't overwrite existing files.
                                if (!writeFile(headerPath, headerContent))
                                    errorMessage = "Could not write component header.";
                                else
                                {
                                    if (hasCppTemplate)
                                    {
                                        if (!writeFile(sourcePath, cppContent))
                                            errorMessage = "Could not write component source.";
                                    }
                                }

                                if (!errorMessage.empty())
                                {
                                    std::error_code cleanupEc;
                                    fs::remove(headerPath, cleanupEc);
                                    if (hasCppTemplate) fs::remove(sourcePath, cleanupEc);
                                }
                                else
                                {
                                    componentName[0] = '\0';
                                    ImGui::CloseCurrentPopup();
                                }
                            }
                        }
                    }
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            componentName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Read current directory contents
    std::vector<fs::directory_entry> entries;
    std::error_code ec;

    fs::directory_iterator it(
        currentDir,
        fs::directory_options::skip_permission_denied,
        ec);

    fs::directory_iterator end;

    while (!ec && it != end)
    {
        const auto& entry = *it;

        std::error_code entryEc;

        // Ignore symbolic links
        if (!entry.is_symlink(entryEc) &&
            !entryEc &&
            isInsideAssets(entry.path()))
        {
            entries.push_back(entry);
        }

        it.increment(ec);
    }

    if (ec)
        errorMessage = ec.message();

    // Sort directories first, then files alphabetically
    std::sort(
        entries.begin(),
        entries.end(),
        [](const fs::directory_entry& a,
           const fs::directory_entry& b)
        {
            std::error_code ecA, ecB;

            bool dirA = a.is_directory(ecA);
            bool dirB = b.is_directory(ecB);

            if (dirA != dirB)
                return dirA;

            return a.path().filename().string() <
                   b.path().filename().string();
        });

    // Scrollable asset content
    ImGui::BeginChild("AssetContent", ImVec2(0, 0), false);

    constexpr float buttonWidth = 120.0f;
    constexpr float buttonHeight = 60.0f;

    float spacing = ImGui::GetStyle().ItemSpacing.x;
    float availableWidth = ImGui::GetContentRegionAvail().x;

    int columns = std::max(
        1,
        static_cast<int>(
            (availableWidth + spacing) /
            (buttonWidth + spacing)));

    ImGui::Columns(columns, "AssetColumns", false);

    for (const auto& entry : entries)
    {
        fs::path path = entry.path();

        std::error_code typeEc;
        bool isDirectory = entry.is_directory(typeEc);

        if (typeEc)
            continue;

        std::string filename = path.filename().string();

        std::string label =
            std::string(isDirectory ? "[DIR] " : "[FILE] ") +
            filename;

        ImGui::PushID(filename.c_str());

        bool isSelected = selectedEntry == path;

        if (isSelected)
        {
            ImGui::PushStyleColor(
                ImGuiCol_Button,
                ImVec4(0.22f, 0.38f, 0.60f, 1.0f));
        }

        ImGui::Button(
            label.c_str(),
            ImVec2(buttonWidth, buttonHeight));

        if (isSelected)
            ImGui::PopStyleColor();

        // Select an item with a single click
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            selectedEntry = path;

        // Open directories with a double click
        if (isDirectory &&
            ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            if (isInsideAssets(path))
            {
                currentDir = path;
                selectedEntry.clear();
            }
        }

        // Item context menu
        if (ImGui::BeginPopupContextItem("FileContext"))
        {
            if (isDirectory && ImGui::MenuItem("Open"))
            {
                if (isInsideAssets(path))
                {
                    currentDir = path;
                    selectedEntry.clear();
                }
            }

            if (ImGui::MenuItem("Delete"))
            {
                pendingDelete = path;
            }

            ImGui::EndPopup();
        }

        ImGui::PopID();
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::EndChild();

    // Delete confirmation dialog
    if (!pendingDelete.empty() &&
        !ImGui::IsPopupOpen("ConfirmDelete"))
    {
        ImGui::OpenPopup("ConfirmDelete");
    }

    if (ImGui::BeginPopupModal(
        "ConfirmDelete",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped(
            "Delete '%s'?",
            pendingDelete.filename().string().c_str());

        ImGui::TextUnformatted(
            "Deleting a folder also deletes its contents.");

        if (ImGui::Button("Delete"))
        {
            if (!pendingDelete.empty() &&
                pendingDelete != rootDir &&
                isInsideAssets(pendingDelete))
            {
                std::error_code deleteEc;
                fs::remove_all(pendingDelete, deleteEc);

                if (deleteEc)
                    errorMessage = deleteEc.message();

                selectedEntry.clear();
            }

            pendingDelete.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            pendingDelete.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Error dialog
    if (!errorMessage.empty())
        ImGui::OpenPopup("AssetError");

    if (ImGui::BeginPopupModal(
        "AssetError",
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextWrapped("%s", errorMessage.c_str());

        if (ImGui::Button("OK"))
        {
            errorMessage.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
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