#pragma once

#include "Controller.h"
#include <glad/glad.h>

class FrameBufferController : public Controller
{
public:
    FrameBufferController();
    ~FrameBufferController();

    bool Init() override;
    bool CleanUp() override;

    void Bind();
    void Unbind();

    void Resize(int width, int height);

    GLuint GetFrameBuffer() const { return _fbo; }
    GLuint GetColorTexture() const { return _colorTexture; }

private:
    GLuint _fbo = 0;
    GLuint _colorTexture = 0;
    GLuint _rbo = 0;

    int _width = 1280;
    int _height = 720;
};