#include "WorldController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

#include "FrameBufferController.h"
#include "SphereCollider.h"
#include "WorldLoader.h"
#include "Object/GameObject/GameObject.h"
#include "Object/GameObject/TestGameObject.h"
#include "Rigidbody/RigidBody.h"
#include "BoxCollider/BoxCollider.h"
#include "Camera/CameraComponent.h"

static std::string ReadTextFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cerr << "[WorldController] File cannot be opened " << fileName << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

WorldController::WorldController(FrameBufferController* frameBuffer) {
    frameBufferController = frameBuffer;
}
WorldController::~WorldController() {}

bool WorldController::Init()
{
    cameraController = new CameraController();
    cameraController->Init();

    SetupShaders();
    //objectTest = new TestGameObject(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(1.0f, 1.0f, 1.0f));
    //objectTest->Start();

    physicsWorld = new PhysicsWorld();

    if (!physicsWorld->Init())
    {
        std::cerr << "[WorldController] Physics init failed!"
                  << std::endl;

        return false;
    }

    lastFrameTime = glfwGetTime();
    physicsAccumulator = 0.0f;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    return true;
}

void WorldController::UpdateLights()
{
    if (!program)
        return;

    const GLuint programID = program->GetProgramId();

    const int lightCount = static_cast<int>(
        std::min(worldLights.size(), size_t(MAX_LIGHTS))
    );

    glUniform1i(
        glGetUniformLocation(programID, "numLights"),
        lightCount
    );

    if (lightCount == 0)
        return;

    std::vector<float> positions;
    std::vector<float> colors;

    positions.reserve(lightCount * 3);
    colors.reserve(lightCount * 3);

    for (int i = 0; i < lightCount; ++i)
    {
        const WorldLights* light = worldLights[i];

        positions.push_back(light->lightPosition.x);
        positions.push_back(light->lightPosition.y);
        positions.push_back(light->lightPosition.z);

        colors.push_back(light->lightColor.x);
        colors.push_back(light->lightColor.y);
        colors.push_back(light->lightColor.z);
    }

    glUniform3fv(
        glGetUniformLocation(programID, "lightPos[0]"),
        lightCount,
        positions.data()
    );

    glUniform3fv(
        glGetUniformLocation(programID, "lightColor[0]"),
        lightCount,
        colors.data()
    );
}

void WorldController::BindView()
{
    targetCamera = nullptr;
    gameFrameBufferController = nullptr;

    for (GameObject* object : GameObjects)
    {
        CameraComponent* camera = GetCameraComponent(object);

        if (camera == nullptr || !camera->Enabled ||
            camera->frameBufferController == nullptr)
            continue;

        if (targetCamera != nullptr && camera->Priority >= targetCamera->Priority)
            continue;

        if (camera != nullptr && camera->Enabled &&
            camera->frameBufferController != nullptr)
        {
            targetCamera = camera;
            gameFrameBufferController = camera->frameBufferController;
        }
    }
}

CameraComponent* WorldController::GetCameraComponent(GameObject* _gameObject)
{
    if (_gameObject == nullptr)
        return nullptr;

    std::vector<CameraComponent*> cameras = _gameObject->GetComponents<CameraComponent>();

    if (cameras.empty())
        return nullptr;

    return cameras[0];
}

update_status WorldController::Update()
{
    if (!program || !cameraController || !frameBufferController)
        return UPDATE_CONTINUE;

    cameraController->Update();
    BindView();

    auto renderPass = [&](FrameBufferController* framebuffer,
                          const Mat4x4& view,
                          const Mat4x4& projection,
                          const Vector3D& cameraPosition)
    {
        if (framebuffer == nullptr)
            return;

        framebuffer->Bind();
        glViewport(0, 0, 1280, 720);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderViewMatrix = view;
        renderProjectionMatrix = projection;

        program->Use();
        const GLuint programID = program->GetProgramId();
        glUniform3f(glGetUniformLocation(programID, "viewPos"),
                    cameraPosition.x, cameraPosition.y, cameraPosition.z);
        glUniform1f(glGetUniformLocation(programID, "ambientStrength"), 0.1f);
        glUniform1f(glGetUniformLocation(programID, "diffuseStrength"), 1.0f);
        glUniform1f(glGetUniformLocation(programID, "specularStrength"), 0.7f);
        glUniform1f(glGetUniformLocation(programID, "shininess"), 32.0f);
        UpdateLights();

        for (GameObject* object : GameObjects)
        {
            if (!object)
                continue;

            UpdateMVP(object);
            object->Update(0.0f);

            if (BoxCollider* collider = object->GetComponent<BoxCollider>())
            {
                const Mat4x4 translation = Mat4x4::Translate(object->position);
                const Mat4x4 rotationX = Mat4x4::RotateX(object->rotation.x);
                const Mat4x4 rotationY = Mat4x4::RotateY(object->rotation.y);
                const Mat4x4 rotationZ = Mat4x4::RotateZ(object->rotation.z);
                const Mat4x4 scale = Mat4x4::Scale(
                    Vector3D(collider->width, collider->height, collider->depth));
                program->Use();
                DrawMatrix(translation, rotationX, rotationY, rotationZ, scale);
                collider->Draw();
            }

            if (SphereCollider* collider = object->GetComponent<SphereCollider>())
            {
                const Mat4x4 translation = Mat4x4::Translate(object->position);
                const Mat4x4 rotationX = Mat4x4::RotateX(object->rotation.x);
                const Mat4x4 rotationY = Mat4x4::RotateY(object->rotation.y);
                const Mat4x4 rotationZ = Mat4x4::RotateZ(object->rotation.z);

                const Mat4x4 scale = Mat4x4::Scale(
                    Vector3D(collider->radius, collider->radius, collider->radius));

                program->Use();
                DrawMatrix(translation, rotationX, rotationY, rotationZ, scale);
                collider->Draw();
            }
        }

        framebuffer->Unbind();
    };

    renderPass(frameBufferController,
               cameraController->GetViewMatrix(),
               cameraController->GetProjMatrix(),
               cameraController->GetCameraPosition());

    if (targetCamera != nullptr && gameFrameBufferController != nullptr &&
        gameFrameBufferController != frameBufferController)
    {
        renderPass(gameFrameBufferController,
                   targetCamera->cameraController->GetViewMatrix(),
                   targetCamera->cameraController->GetProjMatrix(),
                   targetCamera->cameraController->GetCameraPosition());
    }

    const double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - lastFrameTime);
    lastFrameTime = currentTime;
    deltaTime = std::clamp(deltaTime, 0.0f, 0.1f);
    UpdatePhysics(deltaTime);

    return UPDATE_CONTINUE;
}

bool WorldController::CleanUp()
{
    DestroyScene();

    if (physicsWorld) { physicsWorld->CleanUp(); delete physicsWorld; physicsWorld = nullptr; }
    if (vertexShader) { delete vertexShader; vertexShader = nullptr; }
    if (fragmentShader) { delete fragmentShader; fragmentShader = nullptr; }
    if (program) { delete program; program = nullptr; }
    if (cameraController) { delete cameraController; cameraController = nullptr; }

    return true;
}

void WorldController::DestroyScene()
{
    ClearPhysicsBodies();

    for (GameObject*& object : GameObjects)
    {
        if (object == nullptr)
            continue;

        object->Destroy();
        delete object;
        object = nullptr;
    }

    GameObjects.clear();

    if (objectTest != nullptr)
    {
        objectTest->Destroy();
        delete objectTest;
        objectTest = nullptr;
    }
}

void WorldController::SetupShaders()
{
    std::string vsCode = ReadTextFile("vertex.glsl");
    std::string fsCode = ReadTextFile("fragment.glsl");

    vertexShader = new Shader(GL_VERTEX_SHADER, vsCode);
    fragmentShader = new Shader(GL_FRAGMENT_SHADER, fsCode);

    program = new ShaderProgram();
    program->AttachShader(*vertexShader);
    program->AttachShader(*fragmentShader);
    program->Link();

    uMVP_Location = glGetUniformLocation(program->GetProgramId(), "u_MVP");
    if (uMVP_Location == -1)
    {
        std::cerr << "[WorldController] WARNING: 'u_MVP' not found in the shader!" << std::endl;
    }
}

void WorldController::UpdateMVP(GameObject* gameObject)
{
    if (!gameObject || !gameObject->GetComponent<Model>())
        return;

    const Mat4x4 translation = Mat4x4::Translate(gameObject->position);
    const Mat4x4 rotationX = Mat4x4::RotateX(gameObject->rotation.x);
    const Mat4x4 rotationY = Mat4x4::RotateY(gameObject->rotation.y);
    const Mat4x4 rotationZ = Mat4x4::RotateZ(gameObject->rotation.z);
    const Mat4x4 scale = Mat4x4::Scale(gameObject->size);

    DrawMatrix(translation, rotationX, rotationY, rotationZ, scale);
}

void WorldController::DrawMatrix(Mat4x4 translation, Mat4x4 rotationX, Mat4x4 rotationY, Mat4x4 rotationZ, Mat4x4 scale)
{
    const Mat4x4 rotation = rotationX * rotationY * rotationZ;
    modelMatrix = translation * rotation * scale;

    mvp =
        renderProjectionMatrix *
        renderViewMatrix *
        modelMatrix;

    Mat4x4 inverseScale;
    inverseScale.m[0][0] = std::abs(scale.m[0][0]) > 0.000001f ? 1.0f / scale.m[0][0] : 0.0f;
    inverseScale.m[1][1] = std::abs(scale.m[1][1]) > 0.000001f ? 1.0f / scale.m[1][1] : 0.0f;
    inverseScale.m[2][2] = std::abs(scale.m[2][2]) > 0.000001f ? 1.0f / scale.m[2][2] : 0.0f;
    const Mat4x4 normalMatrix = rotation * inverseScale;

    const GLuint programID = program->GetProgramId();

    glUniformMatrix4fv(
        glGetUniformLocation(programID, "normalMatrix"),
        1, GL_FALSE, &normalMatrix.m[0][0]
    );

    glUniformMatrix4fv(
        glGetUniformLocation(programID, "model"),
        1, GL_FALSE, &modelMatrix.m[0][0]
    );

    if (uMVP_Location != -1)
    {
        glUniformMatrix4fv(
            uMVP_Location,
            1, GL_FALSE, &mvp.m[0][0]
        );
    }
}

void WorldController::UpdatePhysics(float deltaTime)
{
    if (!physicsWorld || !physicsWorld->GetWorld())
        return;

    for (GameObject* object : GameObjects)
    {
        if (!object)
            continue;

        RigidBody* rigidBody =
            object->GetComponent<RigidBody>();

        BoxCollider* collider =
            object->GetComponent<BoxCollider>();

        SphereCollider* sphereCollider = object->GetComponent<SphereCollider>();

        if (!rigidBody)
            continue;

        if (!collider && !sphereCollider)
        {
            rigidBody->CleanUpPhysics();
        }
    }

    constexpr float MAX_DELTA_TIME = 0.1f;

    deltaTime = std::clamp(
        deltaTime,
        0.0f,
        MAX_DELTA_TIME
    );

    physicsAccumulator += deltaTime;

    while (physicsAccumulator >= FIXED_TIME_STEP)
    {
        physicsWorld->Update(FIXED_TIME_STEP);
        physicsAccumulator -= FIXED_TIME_STEP;
    }

    SyncPhysicsTransforms();
}

void WorldController::SyncPhysicsTransforms()
{
    for (GameObject* object : GameObjects)
    {
        if (!object)
            continue;

        RigidBody* rigidBody =
            object->GetComponent<RigidBody>();

        if (!rigidBody || !rigidBody->GetBody())
            continue;

        rigidBody->SyncTransform();
    }
}

void WorldController::ClearPhysicsBodies()
{
    for (GameObject* object : GameObjects)
    {
        if (!object)
            continue;

        RigidBody* rigidBody =
            object->GetComponent<RigidBody>();

        if (rigidBody)
            rigidBody->CleanUpPhysics();
    }

    if (objectTest)
    {
        RigidBody* rigidBody =
            objectTest->GetComponent<RigidBody>();

        if (rigidBody)
            rigidBody->CleanUpPhysics();
    }

    physicsAccumulator = 0.0f;
}


bool WorldController::LoadWorld(std::string _fileName)
{
    bool success = false;

    if (!worldLoader)
        worldLoader = new WorldLoader();

    DestroyScene();

    std::vector<GameObject*> objects = worldLoader->GetWorldObjects(_fileName, this);

    for (auto Object: objects)
    {
        Model* model = Object->GetComponent<Model>();

        if (model)
        {
            model->LoadModel(model->FileName, model->TextureName);
        }

        AddGameObject(Object);

        if (Object->parentName != "")
        {
            Object->parent = FindGameObject(Object->parentName);
        }

        Object->Start();

        if (!success)
            success = true;
    }

    return success;
}

GameObject* WorldController::FindGameObject(std::string _name)
{
    GameObject* query = nullptr;

    for (auto object : GameObjects)
    {
        if (object->GetName() == _name)
        {
            query = object;
            break;
        }
    }

    return query;
}

bool WorldController::SaveWorld(std::string _fileName)
{
    bool success = worldLoader->SaveWorld(GameObjects, _fileName);

    return success;
}

void WorldController::AddGameObject(GameObject* _gameObject)
{
    if (_gameObject == nullptr)
        return;

    GameObjects.push_back(_gameObject);
    _gameObject->Start();
}

void WorldController::RemoveLight(int _id)
{
    for (auto it = worldLights.begin(); it != worldLights.end(); ++it)
    {
        if ((*it)->id == _id)
        {
            delete *it;
            worldLights.erase(it);
            break;
        }
    }
}


void WorldController::AddLight(int _id, Vector3D _lightPosition, Vector3D _lightColor)
{
    worldLights.push_back(new WorldLights(_id, _lightPosition, _lightColor));
}

void WorldController::UpdateLight(int _id, Vector3D _lightPosition, Vector3D _lightColor)
{
    for (auto it = worldLights.begin(); it != worldLights.end(); ++it)
    {
        if ((*it)->id == _id)
        {
            (*it)->lightPosition = _lightPosition;
            (*it)->lightColor = _lightColor;
            break;
        }
    }
}