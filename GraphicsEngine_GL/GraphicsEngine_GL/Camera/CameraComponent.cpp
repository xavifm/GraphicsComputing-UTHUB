#include "Object/GameObject/GameObject.h"
#include "CameraComponent.h"
#include "Globals.h"

CameraComponent::CameraComponent()
{
    frameBufferController = new FrameBufferController(1);
    cameraController = new CameraController(false);
    cameraController->Init();
    frameBufferController->Init();
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update()
{
    SerializableComponent<CameraComponent>::Update();

    Vector3D worldPosition = GetAttachedGameObject()->position;
    Vector3D worldRotation = GetAttachedGameObject()->rotation;

    positionX = worldPosition.x + localX;
    positionY = worldPosition.y + localY;
    positionZ = worldPosition.z + localZ;

    rotationX = worldRotation.x + localRX;
    rotationY = worldRotation.y + localRY;
    rotationZ = worldRotation.z + localRZ;

    if (cameraController)
    {
        cameraController->SetCameraPosition(Vector3D(positionX, positionY, positionZ));
        cameraController->SetCameraRotation(Vector3D(rotationX, rotationY, rotationZ));
        cameraController->Update();
    }
}
