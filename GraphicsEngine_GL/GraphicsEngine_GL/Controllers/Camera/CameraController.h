#pragma once
#include "../Controller.h"
#include "../Input/InputController.h"

class CameraController : public Controller
{
public:
    CameraController();
    ~CameraController();

    bool Init() override;
    update_status Update() override;
    void UpdateMVP();
    bool CleanUp() override;

    Mat4x4 LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up);

    Mat4x4 Perspective(float fov, float aspect, float nearPlane, float farPlane);

    Vector3D GetCameraPosition() const { return _cameraPosition; }

    float GetDeltaTime();

    void MoveCamera();
    void RotateCamera();

    Mat4x4 GetViewMatrix() const { return _viewMatrix; }
    Mat4x4 GetProjMatrix() const { return _projMatrix; }

private:

    float _yaw;
    float _pitch;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    Mat4x4 _viewMatrix;
    Mat4x4 _projMatrix;

    float _cameraSpeed;
    float _rotationSpeed;
    float _sensitivity;

    float fov = 30.0f;

    Vector3D _cameraPosition;
    Vector3D _cameraTarget;
    Vector3D _cameraUp;
    Vector3D _cameraFront;

    InputController* input;
};

