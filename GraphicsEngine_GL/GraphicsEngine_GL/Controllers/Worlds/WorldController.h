#pragma once
#include "../Controller.h"
#include "../../Shader/ShaderProgram.h"
#include "../../Model/Model.h"
#include "../../Parameters/Globals.h"
#include "../Input/InputController.h"
#include "../Camera/FrameBufferController.h"
#include "../Camera/CameraController.h"

class GameObject;
//A world hauria d'existir un array de GameObjects, i hauria de cridar el seu update a cada escena (PEX: dibuixo model si el tinc attachat a update), el món s'hauria de carregar per un XML que declari els objectes, o desde la
//UI en càs de voler construïr i que gravi en xml, opció de save world també en XML
//Primer provar de carregar harcodejadament GameObjects i que es puguin gestionar en UI
class WorldController : public Controller
{
public:
    WorldController(FrameBufferController* frameBuffer);
    ~WorldController();

    bool Init() override;
    update_status Update() override;
    bool CleanUp() override;

private:
    void DestroyScene();
    void SetupShaders();
    void UpdateMVP();

    CameraController* cameraController;
    FrameBufferController* frameBufferController;

private:
    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;

    GameObject* objectTest = nullptr;

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
