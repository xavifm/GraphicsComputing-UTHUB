#pragma once

#include <glad/glad.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

class Shader 
{

public:
    Shader(GLenum type, const std::string& source);
    ~Shader();
    GLuint GetId() const;

private:
    GLuint Id;
    void Compile();

};

