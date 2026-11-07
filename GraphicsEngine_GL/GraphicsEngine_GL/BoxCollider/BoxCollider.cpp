#include "BoxCollider.h"

BoxCollider::BoxCollider()
{
    SetupMesh();
}

//he detectat un problema amb la matriu, el primer objecte mana amb totes les rotacions i moviments, s'haurà de separar
BoxCollider::~BoxCollider()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    VAO = 0;
    VBO = 0;
    EBO = 0;
}

void BoxCollider::Update()
{
    SerializableComponent<BoxCollider>::Update();
    SetBoxSizeAndPosition();
    Draw();
}

void BoxCollider::SetupMesh()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BoxCollider::SetBoxSizeAndPosition()
{
    float scaledVertices[24];

    Vector3D position = GetAttachedGameObject()->position;

    for (int i = 0; i < 8; i++)
    {
        scaledVertices[i * 3 + 0] = position.x + (vertices[i * 3 + 0] * width);
        scaledVertices[i * 3 + 1] = position.y + (vertices[i * 3 + 1] * height);
        scaledVertices[i * 3 + 2] = position.z + (vertices[i * 3 + 2] * depth);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(scaledVertices),
        scaledVertices
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BoxCollider::Draw()
{
    glBindVertexArray(VAO);

    glDrawElements(
        GL_LINES,
        24,
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}
