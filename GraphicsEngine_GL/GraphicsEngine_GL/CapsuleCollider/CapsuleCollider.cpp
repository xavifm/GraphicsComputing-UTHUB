#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider()
{
    SetupMesh();
}

CapsuleCollider::~CapsuleCollider()
{
    capsuleMesh.Destroy();
}

void CapsuleCollider::Update()
{
    SerializableComponent<CapsuleCollider>::Update();
    finalHeight = (height / 2) - radius;
}

void CapsuleCollider::SetupMesh()
{
    GenerateCapsuleCollider();
    capsuleMesh.SetData(vertices, texCoords, indices);
}

void CapsuleCollider::Draw()
{
    capsuleMesh.DrawWireframe();
}
