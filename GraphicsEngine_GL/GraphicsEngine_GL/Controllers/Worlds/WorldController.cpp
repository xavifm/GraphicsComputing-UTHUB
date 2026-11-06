#include "WorldController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "FrameBufferController.h"
#include "WorldLoader.h"
#include "Object/GameObject/GameObject.h"
#include "Object/GameObject/TestGameObject.h"
#include "Rigidbody/RigidBody.h"
#include "BoxCollider/BoxCollider.h"

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

update_status WorldController::Update()
{
    if (!program)
        return UPDATE_CONTINUE;

    const double currentTime = glfwGetTime();

    float deltaTime =
        static_cast<float>(currentTime - lastFrameTime);

    lastFrameTime = currentTime;

    deltaTime = std::clamp(deltaTime, 0.0f, 0.1f);

    UpdatePhysics(deltaTime);

    frameBufferController->Bind();

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cameraController->Update();
    program->Use();

    const GLuint programID = program->GetProgramId();

    Vector3D cameraPos = cameraController->GetCameraPosition();

    glUniform3f(
        glGetUniformLocation(programID, "viewPos"),
        cameraPos.x,
        cameraPos.y,
        cameraPos.z
    );

    glUniform1f(
        glGetUniformLocation(programID, "ambientStrength"),
        0.1f
    );

    glUniform1f(
        glGetUniformLocation(programID, "diffuseStrength"),
        1.0f
    );

    glUniform1f(
        glGetUniformLocation(programID, "specularStrength"),
        0.7f
    );

    glUniform1f(
        glGetUniformLocation(programID, "shininess"),
        32.0f
    );

    UpdateLights();

    for (GameObject* object : GameObjects)
    {
        if (!object)
            continue;

        UpdateMVP(object);
        object->Update(0.0f);
    }

    frameBufferController->Unbind();

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
    if (gameObject == nullptr)
        return;

    const Model* model = gameObject->GetComponent<Model>();

    if (model == nullptr)
        return;

    const Mat4x4 translation =
        Mat4x4::Translate(gameObject->position);

    const Mat4x4 rotationX =
        Mat4x4::RotateX(gameObject->rotation.x);

    const Mat4x4 rotationY =
        Mat4x4::RotateY(gameObject->rotation.y);

    const Mat4x4 rotationZ =
        Mat4x4::RotateZ(gameObject->rotation.z);

    const Mat4x4 scale =
        Mat4x4::Scale(gameObject->size);

    modelMatrix =
        translation *
        rotationX *
        rotationY *
        rotationZ *
        scale;

    mvp =
        modelMatrix *
        cameraController->GetViewMatrix() *
        cameraController->GetProjMatrix();

    const Mat4x4 normalMatrix =
        modelMatrix.Inverse().Transpose();

    glUniformMatrix4fv(
        glGetUniformLocation(program->GetProgramId(), "normalMatrix"),
        1,
        GL_FALSE,
        &normalMatrix.m[0][0]);

    glUniformMatrix4fv(
        glGetUniformLocation(program->GetProgramId(), "model"),
        1,
        GL_FALSE,
        &modelMatrix.m[0][0]);

    glUniformMatrix4fv(
        uMVP_Location,
        1,
        GL_FALSE,
        &mvp.m[0][0]);
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

        if (!rigidBody)
            continue;

        if (!collider)
        {
            rigidBody->CleanUpPhysics();
            continue;
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
