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

bool Model::LoadModel(const std::string& fileName, const std::string& textureName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        std::cerr << "[Model] File cannot be opened: " << fileName << std::endl;
        return false;
    }

    std::vector<float> positions;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    std::vector<float> finalVertices;
    std::vector<float> finalTexCoords;

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
        else if (line.rfind("vt ", 0) == 0)
        {
            std::istringstream iss(line.substr(3));
            float u, v;
            iss >> u >> v;
            texCoords.push_back(u);
            texCoords.push_back(1.0f - v);
        }
        else if (line.rfind("f ", 0) == 0)
        {
            std::istringstream iss(line.substr(2));
            std::vector<std::string> faceTokens;
            std::string token;
            while (iss >> token)
                faceTokens.push_back(token);

            auto parseFace = [](const std::string& token, int& vertexIndex, int& texIndex)
                {
                    size_t firstSlash = token.find('/');
                    size_t secondSlash = token.find('/', firstSlash + 1);

                    vertexIndex = std::stoi(token.substr(0, firstSlash)) - 1;
                    texIndex = std::stoi(token.substr(firstSlash + 1, secondSlash - firstSlash - 1)) - 1;
                };

            if (faceTokens.size() == 3 || faceTokens.size() == 4)
            {
                std::vector<int> vertexIndices;
                std::vector<int> texIndices;

                for (const auto& f : faceTokens)
                {
                    int vIdx, tIdx;
                    parseFace(f, vIdx, tIdx);
                    vertexIndices.push_back(vIdx);
                    texIndices.push_back(tIdx);
                }

                for (size_t i = 0; i < 3; ++i)
                {
                    int vIdx = vertexIndices[i] * 3;
                    int tIdx = texIndices[i] * 2;

                    finalVertices.push_back(positions[vIdx]);
                    finalVertices.push_back(positions[vIdx + 1]);
                    finalVertices.push_back(positions[vIdx + 2]);

                    finalTexCoords.push_back(texCoords[tIdx]);
                    finalTexCoords.push_back(texCoords[tIdx + 1]);

                    indices.push_back(static_cast<unsigned int>(finalVertices.size() / 3) - 1);
                }

                if (faceTokens.size() == 4)
                {
                    int vIdx = vertexIndices[0] * 3;
                    int tIdx = texIndices[0] * 2;

                    finalVertices.push_back(positions[vIdx]);
                    finalVertices.push_back(positions[vIdx + 1]);
                    finalVertices.push_back(positions[vIdx + 2]);

                    finalTexCoords.push_back(texCoords[tIdx]);
                    finalTexCoords.push_back(texCoords[tIdx + 1]);

                    indices.push_back(static_cast<unsigned int>(finalVertices.size() / 3) - 1);

                    vIdx = vertexIndices[2] * 3;
                    tIdx = texIndices[2] * 2;

                    finalVertices.push_back(positions[vIdx]);
                    finalVertices.push_back(positions[vIdx + 1]);
                    finalVertices.push_back(positions[vIdx + 2]);

                    finalTexCoords.push_back(texCoords[tIdx]);
                    finalTexCoords.push_back(texCoords[tIdx + 1]);

                    indices.push_back(static_cast<unsigned int>(finalVertices.size() / 3) - 1);

                    vIdx = vertexIndices[3] * 3;
                    tIdx = texIndices[3] * 2;

                    finalVertices.push_back(positions[vIdx]);
                    finalVertices.push_back(positions[vIdx + 1]);
                    finalVertices.push_back(positions[vIdx + 2]);

                    finalTexCoords.push_back(texCoords[tIdx]);
                    finalTexCoords.push_back(texCoords[tIdx + 1]);

                    indices.push_back(static_cast<unsigned int>(finalVertices.size() / 3) - 1);
                }
            }
        }
    }
    file.close();

    Mesh* mesh = new Mesh();
    mesh->SetData(finalVertices, finalTexCoords, indices);
    _mesh_list.push_back(mesh);

    CalcNumVerticesTriangles();
    _texture_attached = SetupTexture(textureName);

    std::cout << "[Model] Loaded " << fileName << " with "
        << _totalVertices << " vertices and "
        << _totalTriangles << " triangles." << std::endl;

    return true;
}

void Model::Draw(unsigned int program) const
{
    if (_texture_attached)
        _texture_attached->Bind();

    for (auto* mesh : _mesh_list)
    {
        mesh->Draw();
    }

    if (_texture_attached)
        _texture_attached->Unbind();
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

Texture* Model::SetupTexture(const std::string textureName)
{
    if (textureName.empty()) return nullptr;

    return new Texture(textureName);
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
