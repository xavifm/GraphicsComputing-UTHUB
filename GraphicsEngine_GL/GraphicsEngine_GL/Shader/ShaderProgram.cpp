#include "ShaderProgram.h"
#include <iostream>

ShaderProgram::ShaderProgram() 
{
    ProgramId = glCreateProgram();
}

ShaderProgram::~ShaderProgram() 
{
    glDeleteProgram(ProgramId);
}

void ShaderProgram::AttachShader(const Shader& shader) 
{
    glAttachShader(ProgramId, shader.GetId());
}

void ShaderProgram::Link() 
{
    glLinkProgram(ProgramId);

    int success;
    glGetProgramiv(ProgramId, GL_LINK_STATUS, &success);

    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(ProgramId, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void ShaderProgram::Use() const 
{
    glUseProgram(ProgramId);
}

