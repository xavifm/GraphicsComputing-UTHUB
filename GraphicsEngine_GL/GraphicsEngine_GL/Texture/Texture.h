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

    bool IsValid() const { return textureID != 0; }

private:
    void LoadTexture(const std::string& filePath);

    GLuint textureID = 0;
    int width, height, numChannels;
};