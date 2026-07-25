#pragma once
#include "GameObject.h"
#include "Model.h"

class TestGameObject : public GameObject
{
public:
    TestGameObject(const Vector3D& position, const Vector3D& size) : GameObject(position, size) {}

    Model* model;

    void Start() override
    {
        model = new Model();
        model->LoadModel("forest_nature_set_all_in.obj", "texture_gradient.png");
        AddComponent(model);
    }

    void Update(float _deltaTime = 0.0f) override
    {
        GameObject::Update(_deltaTime);
        position += Vector3D(0.0f, 0.0f, 0.0f);
        rotation += Vector3D(0.0f, 0.001f, 0.0f);
    }
};
