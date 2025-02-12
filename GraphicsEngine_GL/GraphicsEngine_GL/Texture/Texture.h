#include <glad/glad.h>
#include <Texture/stb_image.h>
#include <string>
#include <iostream>
#include "../Parameters/Globals.h"

class Texture {
public:
    Texture(const std::string& filePath);
    ~Texture();

    void Bind() const;
    void Unbind() const;

    GLuint GetID() const { return textureID; }

private:
    void LoadTexture(const std::string& filePath);

    GLuint textureID;
    int width, height, numChannels;
};