#include "Texture.h"

Texture::Texture(const std::string& filePath) 
{
    LoadTexture(filePath);
}

Texture::~Texture() 
{
    glDeleteTextures(1, &textureID);
}

void Texture::Bind() const 
{
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::Unbind() const 
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadTexture(const std::string& filePath) 
{
    glEnable(GL_TEXTURE_2D);

    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &numChannels, 3);
    if (!data) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
