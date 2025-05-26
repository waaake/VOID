/* System */
// #include <GL/gl.h>
/* Qt */
#include <QMouseEvent>

/* Internal */
#include "Renderer.h"
#include "VoidCore/Logging.h"

#include <chrono>

VOID_NAMESPACE_OPEN

VoidRenderer::VoidRenderer(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_Texture(nullptr)
    , m_ImageData(nullptr)
{
    /* Add Render StatusBar */
    m_RenderStatus = new RendererStatusBar(this);

    /* Enable to track mouse movements */
    setMouseTracking(true);
}

VoidRenderer::~VoidRenderer()
{
    if (m_Texture)
    {
        m_Texture->destroy();
        delete m_Texture;
    }

    /* Delete the Render Status bar */
    m_RenderStatus->deleteLater();
    m_RenderStatus = nullptr;
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

            /* Update the value on the Status bar */
            m_RenderStatus->SetRenderResolution(m_ImageData->Width(), m_ImageData->Height());

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
    /* Adjust the viewport size */
    glViewport(0, 0, w, h);
}

void VoidRenderer::resizeEvent(QResizeEvent* event)
{
    /* Base Resize */
    QOpenGLWidget::resizeEvent(event);

    /* Ensure that the status bar always stays at the bottom of the Renderer */
    m_RenderStatus->move(0, height() - m_RenderStatus->height());
    m_RenderStatus->setFixedWidth(width());
}

void VoidRenderer::mouseMoveEvent(QMouseEvent* event)
{
    /* Fetch the mouse position */
    int x = event->x();
    int y = event->y();

    /* Update the X and Y Coordinates for the mouse movements */
    m_RenderStatus->SetMouseCoordinates(x, y);

    /* Fetch color values at the given point */
    float pixels[4]; /* R G B A*/

    /*
     * TODO: FIX the issue with offset in value
     * Currently there is an offset with which pixel values are read
     * The underlying issue is that we have a dock widget and the left side dock is shriking this widget
     * But then the glViewport(x, y, w, h) call is not happenning correctly causing the pixel values to get offset
     * 
     * Fix is to play without the left dock, but is not very important at this point in development
     * We can come back to this at a later stage to figure out how Qt is handling the calls and come up with a fix accordingly
     */

    glReadPixels(x, height() - y, 1, 1, GL_RGBA, GL_FLOAT, pixels);

    /* Update the Pixel values on the Renderer Status bar */
    m_RenderStatus->SetColourValues(pixels[0], pixels[1], pixels[2], pixels[3]);
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
