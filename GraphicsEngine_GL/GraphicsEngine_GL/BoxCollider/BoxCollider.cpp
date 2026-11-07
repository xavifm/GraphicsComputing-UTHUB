#include "BoxCollider.h"

BoxCollider::BoxCollider()
{
    SetupMesh();
}

BoxCollider::~BoxCollider()
{
    boxMesh.Destroy();
}

void BoxCollider::Update()
{
    SerializableComponent<BoxCollider>::Update();
}

void BoxCollider::SetupMesh()
{
    boxMesh.SetData(vertices, texCoords, indices);
}

void BoxCollider::Draw()
{
    boxMesh.Draw();
}
