#pragma once
#include "../Controller.h"
#include "../../Shader/ShaderProgram.h"
#include "../../Model/Model.h"
#include "../../Parameters/Globals.h"
#include "../Input/InputController.h"
#include "../Camera/FrameBufferController.h"
#include "../Camera/CameraController.h"
#include "../Physics/PhysicsWorld.h"
#include <memory>

class GameObject;
class WorldLoader;

struct WorldLights
{
    WorldLights(int _id, Vector3D _lightPosition, Vector3D _lightColor)
    {
        id = _id;
        lightPosition = _lightPosition;
        lightColor = _lightColor;
    }

    int id;
    Vector3D lightPosition;
    Vector3D lightColor;
};

//A world hauria d'existir un array de GameObjects, i hauria de cridar el seu update a cada escena (PEX: dibuixo model si el tinc attachat a update), el món s'hauria de carregar per un XML que declari els objectes, o desde la
//UI en càs de voler construïr i que gravi en xml, opció de save world també en XML
//Primer provar de carregar harcodejadament GameObjects i que es puguin gestionar en UI
class WorldController : public Controller
{
public:
    WorldController(FrameBufferController* frameBuffer);
    ~WorldController();

    bool Init() override;

    void UpdateLights();

    update_status Update() override;
    bool CleanUp() override;

    void AddGameObject(GameObject* _gameObject);
    void AddLight(int _id, Vector3D _lightPosition, Vector3D _lightColor);

    void UpdateLight(int _id, Vector3D _lightPosition, Vector3D _lightColor);

    void RemoveLight(int _id);
    bool LoadWorld(std::string _fileName);

    GameObject* FindGameObject(std::string _name);

    bool SaveWorld(std::string _fileName);

    std::vector<GameObject*> GameObjects;

    PhysicsWorld* GetPhysicsWorld() { return physicsWorld; };

private:
    void DestroyScene();
    void SetupShaders();
    void UpdateMVP(GameObject* _gameObject);

    WorldLoader* worldLoader;
    PhysicsWorld* physicsWorld;

    CameraController* cameraController;
    FrameBufferController* frameBufferController;

    Shader* vertexShader = nullptr;
    Shader* fragmentShader = nullptr;
    GameObject* objectTest = nullptr;

    Mat4x4 view;
    Mat4x4 proj;
    Mat4x4 worldMatrix;

    unsigned int uMVP_Location = 0;
    float yaw;
    float pitch;

    float physicsAccumulator = 0.0f;
    double lastFrameTime = 0.0;
    static constexpr float FIXED_TIME_STEP = 1.0f / 60.0f;

    void UpdatePhysics(float deltaTime);
    void SyncPhysicsTransforms();
    void ClearPhysicsBodies();

    const int MAX_LIGHTS = 100;

    std::vector<WorldLights*> worldLights;

    Vector3D cameraPosition{ 0.0f, 2.0f, 20.f };
    Vector3D cameraTarget{ 0.0f, 0.0f, 0.0f };
    Vector3D cameraUp{ 0.0f, 1.0f, 0.0f };
};
