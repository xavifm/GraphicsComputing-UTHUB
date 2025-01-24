#pragma once
#include <string>
#include <vector>
#include "../Mesh/Mesh.h"
#include "../Parameters/Globals.h"

class Mesh;

namespace model {
    class Model;
}

class Model {
public:
    Model();
    ~Model();

    bool LoadModel(const std::string& fileName);
    void LoadMaterials(const model::Model& srcModel);
    void Draw(unsigned int program) const;

    void CalcNumVerticesTriangles();
    unsigned int GetTotalTriangles() const { return _totalTriangles; }
    unsigned int GetTotalVertices() const { return _totalVertices; }

    void Destroy();

    void SetPosition(const Vector3D& newPos);
    Vector3D GetPosition() const;

    void SetScale(const Vector3D& newScale);
    Vector3D GetScale() const;

    std::vector<Mesh*> _mesh_list;

private:
    std::vector<unsigned int> _textures;
    unsigned int _totalTriangles, _totalVertices;

    Vector3D Position = Vector3D(0.0f, 0.1f, 1.f);
    Vector3D Scale = Vector3D(0.2f, 0.2f, 0.2f);
};
