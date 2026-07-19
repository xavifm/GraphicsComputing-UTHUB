#pragma once
#include "GameObject.h"
#include "Model.h"

class TestGameObject : public GameObject
{
public:
    TestGameObject(const Vector3D& position, const Vector3D& size) : GameObject(position, size) {}

    std::unique_ptr<Model> model;

    void Start() override
    {
        model = std::make_unique<Model>();
        model->LoadModel("forest_nature_set_all_in.obj", "texture_gradient.png");
        AddComponent(std::move(model));
    }

    void Update(float _deltaTime = 0.0f) override
    {
        GameObject::Update(_deltaTime);
        position += Vector3D(0.0f, 0.0f, 0.0f);
    }
};
