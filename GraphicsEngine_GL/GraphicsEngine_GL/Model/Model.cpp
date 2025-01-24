#include "Model.h"
#include "../Mesh/Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <cstring>

Model::Model() : _totalTriangles(0), _totalVertices(0) 
{
}

Model::~Model() 
{
    Destroy(); 
}

bool Model::LoadModel(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cerr << "[Model] File cannot be opened: " << fileName << std::endl;
        return false;
    }

    std::vector<float> positions;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.rfind("v ", 0) == 0)
        {
            std::istringstream iss(line.substr(2));
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        }
        else if (line.rfind("f ", 0) == 0)
        {
            std::istringstream iss(line.substr(2));
            std::string f1, f2, f3;
            iss >> f1 >> f2 >> f3;

            auto parseIndex = [](const std::string& token) -> unsigned int
                {
                    size_t pos = token.find('/');
                    if (pos == std::string::npos)
                    {
                        return static_cast<unsigned int>(std::stoi(token) - 1);
                    }
                    else
                    {
                        return static_cast<unsigned int>(std::stoi(token.substr(0, pos)) - 1);
                    }
                };

            indices.push_back(parseIndex(f1));
            indices.push_back(parseIndex(f2));
            indices.push_back(parseIndex(f3));
        }
    }
    file.close();

    Mesh* mesh = new Mesh();

    mesh->SetData(positions, indices);

    _mesh_list.push_back(mesh);

    CalcNumVerticesTriangles();

    std::cout << "[Model] Loaded " << fileName << " with "
        << _totalVertices << " vertex and "
        << _totalTriangles << " triangles." << std::endl;

    return true;
}

void Model::LoadMaterials(const model::Model& srcModel)
{

}

void Model::Draw(unsigned int program) const
{
    //textures here

    for (auto* mesh : _mesh_list)
    {
        mesh->Draw();
    }
}

void Model::CalcNumVerticesTriangles()
{
    _totalVertices = 0;
    _totalTriangles = 0;

    for (auto* mesh : _mesh_list)
    {
        _totalVertices += (unsigned int)mesh->GetVertexCount();
        _totalTriangles += (unsigned int)mesh->GetIndexCount() / 3;
    }
}


void Model::Destroy()
{
    for (auto* mesh : _mesh_list)
    {
        if (mesh)
        {
            mesh->Destroy();
            delete mesh;
        }
    }
    _mesh_list.clear();

    _totalTriangles = 0;
    _totalVertices = 0;
}

void Model::SetPosition(const Vector3D& newPos)
{
    Position = newPos;
}

Vector3D Model::GetPosition() const
{
    return Position;
}

void Model::SetScale(const Vector3D& newScale)
{
    Scale = newScale;
}

Vector3D Model::GetScale() const
{
    return Scale;
}
