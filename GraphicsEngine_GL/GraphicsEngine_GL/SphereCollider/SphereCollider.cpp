#include "Object/GameObject/GameObject.h"
#include "SphereCollider.h"
#include "Globals.h"

SphereCollider::SphereCollider()
{
    SetupMesh();
}

SphereCollider::~SphereCollider()
{
    sphereMesh.Destroy();
}

void SphereCollider::Update()
{
    SerializableComponent<SphereCollider>::Update();
}

void SphereCollider::SetupMesh()
{
    GenerateSphere();
    sphereMesh.SetData(vertices, texCoords, indices);
}

void SphereCollider::Draw()
{
    sphereMesh.DrawWireframe();
}
