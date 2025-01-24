#pragma once
#include "../Controller.h"
#include "../../Shader/ShaderProgram.h"
#include "../../Model/Model.h"
#include "../../Parameters/Globals.h"

class WorldController : public Controller
{
public:
    WorldController();
    ~WorldController();

    bool Init() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    void DestroyModel();
    void LoadNewModel(const std::string& fileName);
    void SetupShaders();
    void UpdateMVP();

    Mat4x4 Translate(const Vector3D& pos);

    Mat4x4 Scale(const Vector3D& s);

    Mat4x4 LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up);
    Mat4x4 Perspective(float fov, float aspect, float nearPlane, float farPlane);

private:
    ShaderProgram* program = nullptr;
    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;

    Model* model = nullptr;

    Mat4x4 view;
    Mat4x4 proj;
    Mat4x4 modelMatrix;

    unsigned int uMVP_Location = 0;
    float yaw;
    float pitch;

    Vector3D cameraPosition{ 0.0f, 2.0f, 20.f };
    Vector3D cameraTarget{ 0.0f, 0.0f, 0.0f };
    Vector3D cameraUp{ 0.0f, 1.0f, 0.0f };
};
