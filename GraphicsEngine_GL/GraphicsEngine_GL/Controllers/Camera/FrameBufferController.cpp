#include "FrameBufferController.h"

FrameBufferController::FrameBufferController()
{
}

FrameBufferController::~FrameBufferController()
{
}

bool FrameBufferController::Init()
{
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTexture);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        _width,
        _height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        _colorTexture,
        0
    );

    glGenRenderbuffers(1, &_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        _width,
        _height
    );

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        _rbo
    );

    bool success =
        glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
        GL_FRAMEBUFFER_COMPLETE;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return success;
}

void FrameBufferController::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void FrameBufferController::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferController::Resize(int width, int height)
{
    _width = width;
    _height = height;

    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,
        width,
        height
    );
}

bool FrameBufferController::CleanUp()
{
    if (_rbo)
        glDeleteRenderbuffers(1, &_rbo);

    if (_colorTexture)
        glDeleteTextures(1, &_colorTexture);

    if (_fbo)
        glDeleteFramebuffers(1, &_fbo);

    return true;
}