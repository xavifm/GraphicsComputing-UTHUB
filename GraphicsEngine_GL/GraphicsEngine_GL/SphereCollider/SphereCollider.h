
#pragma once

#include "Mesh.h"
#include "Parameters/Mathx.h"
#include "Component/SerializableComponent.h"

#include <vector>
#include <cmath>

namespace Assets {
    class SphereCollider;
}

class SphereCollider : public SerializableComponent<SphereCollider> {
public:
    SphereCollider();
    ~SphereCollider();

    float radius = 1.0f;

    [[nodiscard]] const char* GetTypeName() const override
    {
        return "SphereCollider";
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
    }

    Mesh sphereMesh;
    Mat4x4 modelMatrix;

    std::vector<float> vertices;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    void GenerateSphere()
    {
        vertices.clear();
        texCoords.clear();
        indices.clear();

        constexpr unsigned int sectors = 36;
        constexpr unsigned int stacks = 18;
        constexpr float PI = 3.14159265359f;

        for (unsigned int i = 0; i <= stacks; ++i)
        {
            float stackAngle = PI / 2.0f - i * PI / stacks;
            float xy = std::cos(stackAngle);
            float z = std::sin(stackAngle);

            for (unsigned int j = 0; j <= sectors; ++j)
            {
                float sectorAngle = j * 2.0f * PI / sectors;

                float x = xy * std::cos(sectorAngle);
                float y = xy * std::sin(sectorAngle);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                texCoords.push_back(
                    static_cast<float>(j) / sectors
                );
                texCoords.push_back(
                    static_cast<float>(i) / stacks
                );
            }
        }

        for (unsigned int i = 0; i < stacks; ++i)
        {
            for (unsigned int j = 0; j < sectors; ++j)
            {
                unsigned int current =
                    i * (sectors + 1) + j;

                unsigned int next =
                    current + sectors + 1;

                indices.push_back(current);
                indices.push_back(current + 1);

                indices.push_back(current);
                indices.push_back(next);
            }
        }
    }
};