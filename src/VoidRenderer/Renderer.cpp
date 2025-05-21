/* System */
// #include <GL/gl.h>

/* Internal */
#include "Renderer.h"

#include <chrono>

VOID_NAMESPACE_OPEN

VoidRenderer::~VoidRenderer()
{
    if (m_Texture)
    {
        m_Texture->destroy();
        delete m_Texture;
    }
}

void VoidRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    ClearFrame();
}

void VoidRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_ImageData && m_ImageData->Data())
    {
        unsigned int texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ImageData->Width(), m_ImageData->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImageData->Data());

        if (texture)
        {   
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);

            /* Maintain aspect ratio */
            double widgetAspect = double(width()) / double(height());
            double imageAspect = double(m_ImageData->Width()) / double((m_ImageData->Height() ? m_ImageData->Height() : 1));

            double factor = widgetAspect / imageAspect;

            glBegin(GL_QUADS);
            glTexCoord2f(0.f, 1.f);
            glVertex3f(-1.f , -1.f * factor, 0.f);

            glTexCoord2f(1.f, 1.f);
            glVertex3f(1.f, -1.f * factor, 0.f);

            glTexCoord2f(1.f, 0.f);
            glVertex3f(1.f, 1.f * factor, 0.f);

            glTexCoord2f(0.f, 0.f);
            glVertex3f(-1.f, 1.f * factor, 0.f);
            glEnd();

            /* Unbind texture */
            glBindTexture(GL_TEXTURE_2D, 0);
            /* Destroy texture */
            glDeleteTextures(1, &texture);
        }
    }
}

void VoidRenderer::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

// void VoidRenderer::Load(const std::string& path)
// {

//     if (m_Texture)
//     {
//         delete m_Texture;
//         m_Texture = nullptr;
//     }

//     LoadFrame(path);

//     // Trigger a Re-paint
//     update();
// }

// void VoidRenderer::Load(const QImage& image)
// {
//     if (m_Texture)
//     {
//         delete m_Texture;
//         m_Texture = nullptr;
//     }

//     LoadFrame(image);

//     /* Trigger a Re-paint */
//     update();
// }

void VoidRenderer::Render(VoidImageData* data)
{
    if (m_Texture)
    {
        delete m_Texture;
        m_Texture = nullptr;
    }

    /* Update the image data */
    m_ImageData = data;

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Play()
{

}

void VoidRenderer::Clear()
{
    /* Delete the reference to the Image Data */
    m_ImageData = nullptr;
    /* Clear the frame */
    ClearFrame();

    /*
     * Trigger a Re-paint
     * This will draw nothing but just clears COLOR and DEPTH
     */
    update();
}

void VoidRenderer::ClearFrame()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

VOID_NAMESPACE_CLOSE
