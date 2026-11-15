#pragma once
#include "../Object/GameObject/GameObject.h"
#include "Component/SerializableComponent.h"
#include "Mesh.h"

namespace Assets {
    class CapsuleCollider;
}

class CapsuleCollider : public SerializableComponent<CapsuleCollider>
{
public:
    CapsuleCollider();
    ~CapsuleCollider();

    float radius = 1.0f;
    float height = 1.0f;
    float finalHeight = 1.0f;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "CapsuleCollider";
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
        visitor("radius", self.radius);
        visitor("height", self.height);
    }

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    Mesh capsuleMesh;
    Mat4x4 modelMatrix;

    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    void GenerateCapsuleCollider(float radius = 0.5f, float height = 2.0f, int segments = 16, int hemisphereRings = 8)
    {
        vertices.clear();
        texCoords.clear();
        indices.clear();

        const float PI = 3.14159265359f;
        const float halfCylinder = std::max(0.0f, height * 0.5f - radius);

        auto addRing = [&](float y, float ringRadius)
        {
            unsigned int start = static_cast<unsigned int>(
                vertices.size() / 3);

            for (int i = 0; i < segments; i++)
            {
                float angle = 2.0f * PI * i / segments;

                vertices.push_back(cos(angle) * ringRadius);
                vertices.push_back(y);
                vertices.push_back(sin(angle) * ringRadius);

                texCoords.push_back(0.0f);
                texCoords.push_back(0.0f);

                if (i > 0)
                {
                    indices.push_back(start + i - 1);
                    indices.push_back(start + i);
                }
            }

            indices.push_back(start + segments - 1);
            indices.push_back(start);

            if (start >= static_cast<unsigned int>(segments))
            {
                for (int i = 0; i < segments; i++)
                {
                    indices.push_back(start - segments + i);
                    indices.push_back(start + i);
                }
            }
        };

        for (int i = 0; i <= hemisphereRings; i++)
        {
            float angle = -PI * 0.5f +
                (PI * 0.5f * i / hemisphereRings);

            float y = -halfCylinder + sin(angle) * radius;
            float r = cos(angle) * radius;

            addRing(y, r);
        }

        for (int i = 0; i <= hemisphereRings; i++)
        {
            float angle = PI * 0.5f * i / hemisphereRings;

            float y = halfCylinder + sin(angle) * radius;
            float r = cos(angle) * radius;

            addRing(y, r);
        }
    }
};
