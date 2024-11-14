#include "Shader.h"

Shader::Shader(GLenum type, const std::string& source)
{
    Id = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(Id, 1, &sourceCStr, nullptr);
    Compile();
}

Shader::~Shader()
{
    glDeleteShader(Id);
}

GLuint Shader::GetId() const
{
    return Id;
}

void Shader::Compile()
{
    glCompileShader(Id);
    int success;
    glGetShaderiv(Id, GL_COMPILE_STATUS, &success);

    if (!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(Id, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}
