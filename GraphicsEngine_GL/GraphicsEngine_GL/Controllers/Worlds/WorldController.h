#pragma once
#include "../Controller.h"
#include "../../Shader/ShaderProgram.h"
#include "../../Model/Model.h"
#include "../../Parameters/Globals.h"
#include "../Input/InputController.h"
#include "../Camera/CameraController.h"

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
    void LoadNewModel(const std::string& fileName, const std::string& textureName = "");
    void SetupShaders();
    void UpdateMVP();

    CameraController* cameraController;

private:
    ShaderProgram* program = nullptr;
    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;

    Model* model = nullptr;

    Mat4x4 view;
    Mat4x4 proj;
    Mat4x4 modelMatrix;
    Mat4x4 mvp;

    unsigned int uMVP_Location = 0;
    float yaw;
    float pitch;

    Vector3D cameraPosition{ 0.0f, 2.0f, 20.f };
    Vector3D cameraTarget{ 0.0f, 0.0f, 0.0f };
    Vector3D cameraUp{ 0.0f, 1.0f, 0.0f };
};
