#include "WorldController.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "../Input/InputController.h"

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
    SetupShaders();

    LoadNewModel("cube.obj");

    cameraPosition = Vector3D(0, 0, 1);
    cameraTarget = Vector3D(0.0f, 0.0f, 0.0f);
    cameraUp = Vector3D(0.0f, 1.0f, 0.0f);

    modelMatrix = Mat4x4();

    view = LookAt(cameraPosition, cameraTarget, cameraUp);

    float aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    proj = Perspective(60.0f, aspect, 0.1f, 100.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return true;
}

//update_status WorldController::Update()
//{
//    if (!program) 
//        return UPDATE_CONTINUE;
//
//    program->Use();
//
//    UpdateMVP();
//
//    if (model)
//    {
//        model->Draw(program->GetProgramId());
//    }
//
//    return UPDATE_CONTINUE;
//}

update_status WorldController::Update()
{
    if (!program)
        return UPDATE_CONTINUE;

    // 1) Agafem l'InputController
    // (idealment hauries de reutilitzar l'InputController instanciat per l'Application,
    //  però per a l'exemple ho deixem així)
    InputController* input = new InputController();
    input->PreUpdate();

    // 2) Definim la sensibilitat de càmera
    float cameraSpeed = 0.1f;  // Velocitat de translació (WASD i 8/5)

    // 3) Calculem el vector forward i right
    Vector3D forwardDir = (cameraTarget - cameraPosition).normalized();
    Vector3D right = forwardDir.cross(cameraUp).normalized();

    // 4) WASD
    if (input->GetKeyboardKey(GLFW_KEY_W, GLFW_PRESS)) {
        cameraPosition = cameraPosition + forwardDir * cameraSpeed;
        cameraTarget = cameraTarget + forwardDir * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_S, GLFW_PRESS)) {
        cameraPosition = cameraPosition - forwardDir * cameraSpeed;
        cameraTarget = cameraTarget - forwardDir * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_A, GLFW_PRESS)) {
        cameraPosition = cameraPosition - right * cameraSpeed;
        cameraTarget = cameraTarget - right * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_D, GLFW_PRESS)) {
        cameraPosition = cameraPosition + right * cameraSpeed;
        cameraTarget = cameraTarget + right * cameraSpeed;
    }

    // 5) Moviment amunt (8) / avall (5) en l'eix Y
    //    afegim o restem vector(0, 1, 0)
    if (input->GetKeyboardKey(GLFW_KEY_8, GLFW_PRESS)) {
        Vector3D up(0.0f, 1.0f, 0.0f);
        cameraPosition = cameraPosition + up * cameraSpeed;
        cameraTarget = cameraTarget + up * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_5, GLFW_PRESS)) {
        Vector3D down(0.0f, -1.0f, 0.0f);
        cameraPosition = cameraPosition + down * cameraSpeed;
        cameraTarget = cameraTarget + down * cameraSpeed;
    }

    // 6) Recalculem la matriu de vista
    view = LookAt(cameraPosition, cameraTarget, cameraUp);

    // 7) Neteja i dibuix
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->Use();
    UpdateMVP();

    if (model)
        model->Draw(program->GetProgramId());

    return UPDATE_CONTINUE;
}




bool WorldController::CleanUp()
{
    DestroyModel();

    if (vertexShader) { delete vertexShader; vertexShader = nullptr; }
    if (fragmentShader) { delete fragmentShader; fragmentShader = nullptr; }
    if (program) { delete program; program = nullptr; }

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

void WorldController::LoadNewModel(const std::string& fileName)
{
    DestroyModel();
    model = new Model();
    if (!model->LoadModel(fileName))
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
        std::cerr << "[WorldController] WARNING: 'u_MVP' not found to the shader!" << std::endl;
    }
}

void WorldController::UpdateMVP()
{
    Vector3D position = model->GetPosition();
    Vector3D scale = model->GetScale();

    Mat4x4 translation = Translate(position);
    Mat4x4 scaleMat = Scale(scale);

    modelMatrix = translation * scaleMat;

    Mat4x4 mvp = modelMatrix * view * proj;
    glUniformMatrix4fv(uMVP_Location, 1, GL_FALSE, &mvp.m[0][0]);
}

//globals millor
Mat4x4 WorldController::Translate(const Vector3D& pos)
{
    Mat4x4 mat;
    mat.m[3][0] = pos.x;
    mat.m[3][1] = pos.y;
    mat.m[3][2] = pos.z;
    return mat;
}

Mat4x4 WorldController::Scale(const Vector3D& s)
{
    Mat4x4 mat;
    mat.m[0][0] = s.x;
    mat.m[1][1] = s.y;
    mat.m[2][2] = s.z;
    return mat;
}
//globals millor

Mat4x4 WorldController::LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up)
{
    Vector3D f = (center - eye).normalized();
    Vector3D u = up.normalized();
    Vector3D s = f.cross(u).normalized();
    u = s.cross(f);

    Mat4x4 result;

    result.m[0][0] = s.x;  result.m[0][1] = s.y;  result.m[0][2] = s.z;  result.m[0][3] = 0.0f;
    result.m[1][0] = u.x;  result.m[1][1] = u.y;  result.m[1][2] = u.z;  result.m[1][3] = 0.0f;
    result.m[2][0] = -f.x; result.m[2][1] = -f.y; result.m[2][2] = -f.z; result.m[2][3] = 0.0f;
    result.m[3][0] = 0.0f; result.m[3][1] = 0.0f; result.m[3][2] = 0.0f; result.m[3][3] = 1.0f;

    Mat4x4 translation;
    translation.m[3][0] = -eye.x;
    translation.m[3][1] = -eye.y;
    translation.m[3][2] = -eye.z;

    return result * translation;
}

Mat4x4 WorldController::Perspective(float fov, float aspect, float nearPlane, float farPlane)
{
    Mat4x4 result;

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            result.m[i][j] = 0.0f;

    float tanHalfFOV = std::tan((fov * 3.1415926535f / 180.0f) / 2.0f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFOV);
    result.m[1][1] = 1.0f / tanHalfFOV;
    result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

    return result;
}
