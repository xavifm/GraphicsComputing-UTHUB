#include "CameraController.h"
#include <GLFW/glfw3.h>
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

CameraController::CameraController() : _yaw(-90.0f), _pitch(0.0f) {}
CameraController::~CameraController() {}

bool CameraController::Init()
{
    _cameraPosition = Vector3D(0, 0, 1);
    _cameraTarget = Vector3D(0.0f, 0.0f, 0.0f);
    _cameraFront = Vector3D(0.0f, 0.0f, -1.0f);
    _cameraUp = Vector3D(0.0f, 1.0f, 0.0f);

    _viewMatrix = LookAt(_cameraPosition, _cameraTarget + _cameraFront, _cameraUp);

    float aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    _projMatrix = Perspective(fov, aspect, 0.1f, 100.0f);

    input = new InputController();

    return true;
}

update_status CameraController::Update()
{
    input->PreUpdate();

    float cameraSpeed = 5.0f * GetDeltaTime();
    float mouseSensitivity = 0.1f;

    Vector3D right = _cameraFront.cross(_cameraUp).normalized();

    if (input->GetKeyboardKey(GLFW_KEY_W, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition + _cameraFront * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_S, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition - _cameraFront * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_A, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition + right * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_D, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition - right * cameraSpeed;
    }

    if (input->GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS)) {
        Vector2D delta = input->GetMouseMotion();

        _yaw -= delta.x * mouseSensitivity;
        _pitch += delta.y * mouseSensitivity;

        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;

        Vector3D newFront;
        newFront.x = cos(_yaw * M_PI / 180.0f) * cos(_pitch * M_PI / 180.0f);
        newFront.y = sin(_pitch * M_PI / 180.0f);
        newFront.z = sin(_yaw * M_PI / 180.0f) * cos(_pitch * M_PI / 180.0f);
        _cameraFront = newFront.normalized();
    }

    _viewMatrix = LookAt(_cameraPosition, _cameraPosition + _cameraFront, _cameraUp);

    return UPDATE_CONTINUE;
}


bool CameraController::CleanUp()
{
    return true;
}

Mat4x4 CameraController::LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up)
{
    Vector3D f = (center - eye).normalized();
    Vector3D s = up.cross(f).normalized();
    Vector3D u = f.cross(s);

    Mat4x4 result;
    result.m[0][0] = s.x; result.m[1][0] = s.y; result.m[2][0] = s.z; result.m[3][0] = -s.dot(eye);
    result.m[0][1] = u.x; result.m[1][1] = u.y; result.m[2][1] = u.z; result.m[3][1] = -u.dot(eye);
    result.m[0][2] = -f.x; result.m[1][2] = -f.y; result.m[2][2] = -f.z; result.m[3][2] = f.dot(eye);
    result.m[0][3] = 0.0f; result.m[1][3] = 0.0f; result.m[2][3] = 0.0f; result.m[3][3] = 1.0f;

    return result;
}


Mat4x4 CameraController::Perspective(float fov, float aspect, float nearPlane, float farPlane)
{
    Mat4x4 result = {};
    float tanHalfFOV = std::tan((fov * 3.1415926535f / 180.0f) / 2.0f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFOV);
    result.m[1][1] = 1.0f / tanHalfFOV;
    result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    result.m[2][3] = -1.0f;
    result.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

    return result;
}

float CameraController::GetDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    return deltaTime;
}

