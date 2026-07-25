#pragma once
#include <memory>
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

    bool LoadEmptyModel();
    bool LoadModel(const std::string& fileName, const std::string& textureName = "");
    void Draw(unsigned int program) const;

    bool ModelIsLoaded = false;

    void CalcNumVerticesTriangles();
    unsigned int GetTotalTriangles() const { return _totalTriangles; }
    unsigned int GetTotalVertices() const { return _totalVertices; }

    void Destroy() override;

    std::unique_ptr<Texture> SetupTexture(const std::string &textureName);

    void SetPosition(const Vector3D& newPos);
    Vector3D GetPosition() const;

    void SetScale(const Vector3D& newScale);
    Vector3D GetScale() const;

    std::vector<Mesh*> _mesh_list;

    std::unique_ptr<Texture> _texture_attached;

    void Update() override;

private:
    std::vector<unsigned int> _textures;
    unsigned int _totalTriangles, _totalVertices;
};
