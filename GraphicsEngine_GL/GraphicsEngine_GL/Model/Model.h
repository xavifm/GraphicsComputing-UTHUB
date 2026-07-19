#pragma once
#include <string>
#include <vector>
#include "../Mesh/Mesh.h"
#include "../Parameters/Globals.h"
#include "../Texture/Texture.h"
#include "Component/Component.h"

class Mesh;

namespace model {
    class Model;
}

class Model : public Component
{
public:
    Model();
    ~Model();

    bool LoadModel(const std::string& fileName, const std::string& textureName = "");
    void Draw(unsigned int program) const;

    void CalcNumVerticesTriangles();
    unsigned int GetTotalTriangles() const { return _totalTriangles; }
    unsigned int GetTotalVertices() const { return _totalVertices; }

    void Destroy() override;

    Texture* SetupTexture(const std::string textureName);
    void SetPosition(const Vector3D& newPos);
    Vector3D GetPosition() const;

    void SetScale(const Vector3D& newScale);
    Vector3D GetScale() const;

    std::vector<Mesh*> _mesh_list;

    Texture* _texture_attached;

    void Update() override;

private:
    std::vector<unsigned int> _textures;
    unsigned int _totalTriangles, _totalVertices;
};
