#pragma once
#include "../Object/GameObject/GameObject.h"
#include "Component/SerializableComponent.h"
#include "Mesh.h"

namespace Assets {
    class BoxCollider;
}

class BoxCollider : public SerializableComponent<BoxCollider>
{
public:
    BoxCollider();
    ~BoxCollider();

    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "BoxCollider";
    }

    template<class Visitor>
    void Fields(Visitor& visitor)
    {
        VisitFields(*this, visitor);
    }

    template<class Visitor>
    void Fields(Visitor& visitor) const
    {
        VisitFields(*this, visitor);
    }

    void Update();

    void SetupMesh();

    void Draw();

private:
    template<class Self, class Visitor>
    static void VisitFields(Self& self, Visitor& visitor)
    {
        visitor("width", self.width);
        visitor("height", self.height);
        visitor("depth", self.depth);
    }

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    Mesh boxMesh;
    Mat4x4 modelMatrix;

    std::vector<float> vertices =
    {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f
    };

    std::vector<float> texCoords =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    std::vector<unsigned int> indices =
    {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };
};