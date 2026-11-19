#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider()
{
    SetupMesh(radius, height);
}

CapsuleCollider::~CapsuleCollider()
{
    capsuleMesh.Destroy();
}

void CapsuleCollider::Update()
{
    SerializableComponent<CapsuleCollider>::Update();
}

void CapsuleCollider::SetupMesh(float _radius, float _height)
{
    GenerateCapsuleCollider(_radius, _height);

    capsuleMesh.SetData(vertices, texCoords, indices);
}

void CapsuleCollider::Draw()
{
    capsuleMesh.DrawWireframe();
}
