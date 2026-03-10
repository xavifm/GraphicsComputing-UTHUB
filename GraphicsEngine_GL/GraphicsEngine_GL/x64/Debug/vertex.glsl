#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 u_MVP;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));

    Normal = normalize(normalMatrix * aNormal);

    TexCoords = aTexCoord;

    gl_Position = u_MVP * vec4(aPos, 1.0);
}
