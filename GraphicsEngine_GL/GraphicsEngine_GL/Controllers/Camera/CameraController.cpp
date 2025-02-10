#include "CameraController.h"
#include <GLFW/glfw3.h>
#include "../Input/InputController.h"

CameraController::CameraController() {}
CameraController::~CameraController() {}

bool CameraController::Init()
{
    _cameraPosition = Vector3D(0, 0, 1);
    _cameraTarget = Vector3D(0.0f, 0.0f, 0.0f);
    _cameraUp = Vector3D(0.0f, 1.0f, 0.0f);

    _viewMatrix = LookAt(_cameraPosition, _cameraTarget, _cameraUp);

    float aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    _projMatrix = Perspective(60.0f, aspect, 0.1f, 100.0f);

    _viewMatrix = LookAt(_cameraPosition, _cameraTarget, _cameraUp);

    return true;
}

update_status CameraController::Update()
{
    InputController* input = new InputController();
    input->PreUpdate();

    float cameraSpeed = 0.1f;
    Vector3D forwardDir = (_cameraTarget - _cameraPosition).normalized();
    Vector3D right = forwardDir.cross(_cameraUp).normalized();

    if (input->GetKeyboardKey(GLFW_KEY_W, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition + forwardDir * cameraSpeed;
        _cameraTarget = _cameraTarget + forwardDir * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_S, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition - forwardDir * cameraSpeed;
        _cameraTarget = _cameraTarget - forwardDir * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_A, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition - right * cameraSpeed;
        _cameraTarget = _cameraTarget - right * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_D, GLFW_PRESS)) {
        _cameraPosition = _cameraPosition + right * cameraSpeed;
        _cameraTarget = _cameraTarget + right * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_8, GLFW_PRESS)) {
        Vector3D up(0.0f, 1.0f, 0.0f);
        _cameraPosition = _cameraPosition + up * cameraSpeed;
        _cameraTarget = _cameraTarget + up * cameraSpeed;
    }
    if (input->GetKeyboardKey(GLFW_KEY_5, GLFW_PRESS)) {
        Vector3D down(0.0f, -1.0f, 0.0f);
        _cameraPosition = _cameraPosition + down * cameraSpeed;
        _cameraTarget = _cameraTarget + down * cameraSpeed;
    }

    _viewMatrix = LookAt(_cameraPosition, _cameraTarget, _cameraUp);

    return UPDATE_CONTINUE;
}

bool CameraController::CleanUp()
{
    return true;
}

Mat4x4 CameraController::LookAt(const Vector3D& eye, const Vector3D& center, const Vector3D& up)
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

