#include "Light.h"
#include <climits>
#include <random>

#include "WorldController.h"
#include "Object/GameObject/GameObject.h"

Light::Light()
{
    Id = -1;
    StartedLight = false;

    Position = Vector3D(0.0f, 0.0f, 0.0f);
    Color = Vector3D(1.0f, 1.0f, 1.0f);
}

Light::~Light()
{
    GameObject* gameObject = GetAttachedGameObject();

    if (StartedLight && gameObject && gameObject->worldController)
    {
        gameObject->worldController->RemoveLight(Id);
    }
}


void Light::Update()
{
    SerializableComponent<Light>::Update();

    if (!StartedLight)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> dist(1, INT_MAX);

        Id = dist(gen);

        GetAttachedGameObject()->worldController->AddLight(Id, Position, Color);
        StartedLight = true;
    }
    else
    {
        Position = GetAttachedGameObject()->position;
        GetAttachedGameObject()->worldController->UpdateLight(Id, Position, Color);
    }
}
