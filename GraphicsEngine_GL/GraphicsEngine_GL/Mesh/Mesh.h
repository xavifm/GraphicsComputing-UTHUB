#pragma once
#include <vector>
#include <glad/glad.h>
#include <iostream>

class Mesh
{
public:
    Mesh() : VAO(0), VBO(0), EBO(0) {}
    ~Mesh() { Destroy(); }

    void SetData(const std::vector<float>& vertexData, const std::vector<unsigned int>& indices);
    void Draw() const;
    void Destroy();

    size_t GetVertexCount() const { return vertexCount; }
    size_t GetIndexCount()  const { return indexCount; }

private:
    void SetupMesh();

    unsigned int VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> inds;

    size_t vertexCount = 0;
    size_t indexCount = 0;
};

