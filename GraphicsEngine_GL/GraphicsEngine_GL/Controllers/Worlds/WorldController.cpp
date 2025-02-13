#include "WorldController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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

WorldController::WorldController() {}
WorldController::~WorldController() {}

bool WorldController::Init()
{   
    cameraController = new CameraController();
    cameraController->Init();
    
    SetupShaders();
    LoadNewModel("forest_nature_set_all_in.obj", "texture_gradient.png");
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE); 

    
    return true;
}

update_status WorldController::Update()
{
    if (!program) 
        return UPDATE_CONTINUE;

    cameraController->Update();
    program->Use();

    UpdateMVP();
    
    if (model)
    {
        model->Draw(program->GetProgramId());
    }

    return UPDATE_CONTINUE;
}

bool WorldController::CleanUp()
{
    DestroyModel();
    
    if (vertexShader) { delete vertexShader; vertexShader = nullptr; }
    if (fragmentShader) { delete fragmentShader; fragmentShader = nullptr; }
    if (program) { delete program; program = nullptr; }
    if (cameraController) { delete cameraController; cameraController = nullptr; }
    
    return true;
}

void WorldController::DestroyModel()
{
    if (model)
    {
        model->Destroy();
        delete model;
        model = nullptr;
    }
}

void WorldController::LoadNewModel(const std::string& fileName, const std::string& textureName)
{
    DestroyModel();
    model = new Model();
    if (!model->LoadModel(fileName, textureName))
    {
        std::cerr << "[WorldController] Error loading the model: " << fileName << std::endl;
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
    Vector3D position = model->GetPosition();
    Vector3D scale = model->GetScale();
    
    Mat4x4 translation = Mat4x4::Translate(position);
    Mat4x4 scaleMat = Mat4x4::Scale(scale);
    
    modelMatrix = translation * scaleMat;
    
    Mat4x4 mvp = cameraController->GetViewMatrix() * cameraController->GetProjMatrix() * modelMatrix;
    glUniformMatrix4fv(uMVP_Location, 1, GL_FALSE, &mvp.m[0][0]);
}
