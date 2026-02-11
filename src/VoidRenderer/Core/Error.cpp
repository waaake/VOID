// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <string>

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "Error.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

namespace Renderer {

void GLGetError()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        switch (err)
        {
            case GL_INVALID_ENUM:
                VOID_LOG_ERROR("GL Invalid enum.");
                break;
            case GL_INVALID_VALUE:
                VOID_LOG_ERROR("GL Invalid value.");
                break;
            case GL_INVALID_OPERATION:
                VOID_LOG_ERROR("GL Invalid operation.");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                VOID_LOG_ERROR("GL Invalid Framebuffer operation.");
                break;
            case GL_OUT_OF_MEMORY:
                VOID_LOG_ERROR("GL out of memory.");
                break;
            default:
                VOID_LOG_ERROR("Unknown GL Error: {0}", err);
        }
    }
}

void GLGetError(const std::string& base)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        switch (err)
        {
            case GL_INVALID_ENUM:
                VOID_LOG_ERROR("{0}::GL Invalid enum.", base);
                break;
            case GL_INVALID_VALUE:
                VOID_LOG_ERROR("{0}::GL Invalid value.", base);
                break;
            case GL_INVALID_OPERATION:
                VOID_LOG_ERROR("{0}::GL Invalid operation.", base);
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                VOID_LOG_ERROR("{0}::GL Invalid Framebuffer operation.", base);
                break;
            case GL_OUT_OF_MEMORY:
                VOID_LOG_ERROR("{0}::GL out of memory.", base);
                break;
            default:
                VOID_LOG_ERROR("{0}::Unknown GL Error: {1}", base, err);
        }
    }
}

} // namespace Renderer

VOID_NAMESPACE_CLOSE
