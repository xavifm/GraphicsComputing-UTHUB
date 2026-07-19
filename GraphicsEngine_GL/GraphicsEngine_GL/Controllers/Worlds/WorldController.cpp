#include "WorldController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "FrameBufferController.h"
#include "Object/GameObject/GameObject.h"
#include "Object/GameObject/TestGameObject.h"

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
    objectTest = new TestGameObject(Vector3D(0.0f, 0.0f, 0.0f), Vector3D(1.0f, 1.0f, 1.0f));
    objectTest->Start();
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    
    return true;
}

update_status WorldController::Update()
{
    if (!program)
        return UPDATE_CONTINUE;

    frameBufferController->Bind();

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cameraController->Update();
    program->Use();

    UpdateMVP();

    Vector3D lightOffset(5.0f, 5.0f, 5.0f);
    Vector3D lightPosition = lightOffset;

    glUniform3f(glGetUniformLocation(program->GetProgramId(), "lightPos"), lightPosition.x, lightPosition.y, lightPosition.z);
    glUniform3f(glGetUniformLocation(program->GetProgramId(), "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program->GetProgramId(), "viewPos"),
        cameraController->GetCameraPosition().x,
        cameraController->GetCameraPosition().y,
        cameraController->GetCameraPosition().z);

    glUniform1f(glGetUniformLocation(program->GetProgramId(), "ambientStrength"), 1.f);
    glUniform1f(glGetUniformLocation(program->GetProgramId(), "specularStrength"), 0.7f);
    glUniform1f(glGetUniformLocation(program->GetProgramId(), "shininess"), 32.0f);


    Mat4x4 normalMatrix = modelMatrix.Inverse().Transpose();
    glUniformMatrix4fv(glGetUniformLocation(program->GetProgramId(), "normalMatrix"), 1, GL_FALSE, &normalMatrix.m[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(program->GetProgramId(), "model"), 1, GL_FALSE, &modelMatrix.m[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program->GetProgramId(), "u_MVP"), 1, GL_FALSE, &mvp.m[0][0]);

    objectTest->Update(0.0f);

    frameBufferController->Unbind();

    return UPDATE_CONTINUE;
}

bool WorldController::CleanUp()
{
    DestroyScene();
    
    if (vertexShader) { delete vertexShader; vertexShader = nullptr; }
    if (fragmentShader) { delete fragmentShader; fragmentShader = nullptr; }
    if (program) { delete program; program = nullptr; }
    if (cameraController) { delete cameraController; cameraController = nullptr; }
    
    return true;
}

void WorldController::DestroyScene()
{
    if (objectTest)
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

void WorldController::UpdateMVP()
{
    if (!objectTest)
        return;

    Mat4x4 translation = Mat4x4::Translate(objectTest->position);
    Mat4x4 rotationX = Mat4x4::RotateX(objectTest->rotation.x);
    Mat4x4 rotationY = Mat4x4::RotateY(objectTest->rotation.y);
    Mat4x4 rotationZ = Mat4x4::RotateZ(objectTest->rotation.z);
    Mat4x4 scaleMatrix = Mat4x4::Scale(objectTest->size);

    modelMatrix = translation * rotationX * rotationY * rotationZ * scaleMatrix;

    mvp = modelMatrix * cameraController->GetViewMatrix() * cameraController->GetProjMatrix();

    glUniformMatrix4fv(uMVP_Location,1, GL_FALSE, &mvp.m[0][0]);
}
