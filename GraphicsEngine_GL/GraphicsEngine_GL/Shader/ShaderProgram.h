#pragma once

#include <glad/glad.h>
#include "Shader.h"

class ShaderProgram 
{

public:
    ShaderProgram();
    ~ShaderProgram();
    void AttachShader(const Shader& shader);
    void Link();
    void Use() const;

private:
    GLuint ProgramId;

};
