/* System */
// #include <GL/gl.h>

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>     // for ortho/scale
#include <glm/gtc/type_ptr.hpp>             // for glm::value_ptr

/* Qt */
#include <QMouseEvent>

/* Internal */
#include "Renderer.h"
#include "VoidCore/Logging.h"

static const float MAX_ZOOM = 12.8;
static const float MIN_ZOOM = 0.1;

VOID_NAMESPACE_OPEN

VoidRenderer::VoidRenderer(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_Texture(nullptr)
    , m_ImageData(nullptr)
    , m_ZoomFactor(1.f)
    , m_TranslateX(0.f)
    , m_TranslateY(0.f)
    , m_Pressed(false)
    , m_Pan(0.f, 0.f)
{
    /* Add Render StatusBar */
    m_RenderStatus = new RendererStatusBar(this);
    /* Message display */
    m_DisplayLabel = new RendererDisplayLabel(this);
    m_DisplayLabel->setVisible(false);

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

    /* Delete the Error Label */
    m_DisplayLabel->deleteLater();
    m_DisplayLabel = nullptr;
}

void VoidRenderer::initializeGL()
{
    /* Create OpenGL Context and Initialize shaders */
    Initialize();

    /* Enable Blending */
    glEnable(GL_BLEND);

    /**
     * Maths for blending
     *  r x source alpha + rdest x (1 - source alpha) = R
     *  g x source alpha + gdest x (1 - source alpha) = G
     *  b x source alpha + bdest x (1 - source alpha) = B
     *  a x source alpha + adest x (1 - source alpha) = A
     *
     * RGBA is the final value which will be rendered
     */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /**
     * Quad vertices with texture coords
     * Create Vertex Attrib Object and Vertex Buffer Objects
     */
    float vertices[16] = {
        // Positions  // Texture Coords
        -1.f, -1.f,  0.f,  1.f,
         1.f, -1.f,  1.f,  1.f,
         1.f,  1.f,  1.f,  0.f,
        -1.f,  1.f,  0.f,  0.f,
    };

    /**
     * Index/Element Buffer indices
     * Tells GL how to draw the triangles
     */
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    /**
     * Generate the vertex array object names
     * Generate 1 array
     */
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void VoidRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_ImageData && m_ImageData->Data())
    {
        /* Texture ID */
        unsigned int texture;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        /**
         * Bind the shader program to use for
         */
        Bind();

        glGenTextures(1, &texture);
        /* Bind the Generated texture for Render */
        glBindTexture(GL_TEXTURE_2D, texture);

        /**
         * Load the image data onto the Texture 2D
         */
        /* Use the appropriate format */
        int format = m_ImageData->Channels() == 3 ? GL_RGB : GL_RGBA;
        /* Specify the 2D texture image to be read by the vertex and fragment shaders */
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_ImageData->Width(), m_ImageData->Height(), 0, format, GL_UNSIGNED_BYTE, m_ImageData->Data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        /**
         * To ensure the image is of the correct aspect while render
         * Calculate the aspect of the current view (Renderer Width / Renderer Height)
         * And the aspect of the image being rendered
         */
        float viewAspect = float(width()) / float(height());
        float imageAspect = float(m_ImageData->Width()) / float((m_ImageData->Height() ? m_ImageData->Height() : 1));

        /* Find the overall scale of the image */
        glm::vec2 scale = (imageAspect > viewAspect) ? glm::vec2(1.f, viewAspect / imageAspect) : glm::vec2(imageAspect / viewAspect, 1.f);

        /**
         * Get the Model matrix,
         * This is how our image/model looks like as a 4x4 matrix
         */
        // glm::mat4 model = glm::scale(glm::mat4(1.f), glm::vec3(scale.x * m_ZoomFactor, scale.y * m_ZoomFactor, 1.f));
        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(m_Pan, 0.f));

        /* Update the model with the aspect and the zoom scale */
        model = glm::scale(model, glm::vec3(scale.x * m_ZoomFactor, scale.y * m_ZoomFactor, 1.f));

        /**
         * And the projection matrix of how it's supposed to be projected on the viewport
         * Holds the scaling and aspect
         */
        glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f);

        /*
         * Calculate the model view prohection matrix
         * For any 2D Texture/image the transform from the camera is unity
         */
        glm::mat4 mvp = model * projection; // * unity view i.e. glm::mat4(1.f)

        GLuint projLoc = glGetUniformLocation(ProgramId(), "uMVP");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        /**
         * Draw triangles as bound int the Index buffer as defined earlier
         *  3 ___ 2
         *   |  /|
         *   | / |
         *   |/__|
         *  0     1
         *
         * 0 - 1 - 2
         * 2 - 3 - 0
         */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /**
         * Rendering is complete
         * Cleanup....
         */
        /* Unbind texture */
        glBindTexture(GL_TEXTURE_2D, 0);
        /* Destroy texture */
        glDeleteTextures(1, &texture);

        /* Release the active shader program from the current OpenGL Context */
        Release();
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

    /* The Label has to have a certain gap from the edges */
    m_DisplayLabel->move(10, 10);

    /* Ensure that the status bar always stays at the bottom of the Renderer */
    m_RenderStatus->move(0, height() - m_RenderStatus->height());
    m_RenderStatus->setFixedWidth(width());
}

void VoidRenderer::mousePressEvent(QMouseEvent* event)
{
    /* Indicates that the mouse has been pressed */
    m_Pressed = true;

    /* Save the current mouse pos */
    #if _QT6    /* Qt6 Compat */
    m_LastMouse = event->position();
    #else
    m_LastMouse = event->pos();
    #endif // _QT6
}

void VoidRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    /* And indicates that the mouse was released */
    m_Pressed = false;
}

void VoidRenderer::mouseMoveEvent(QMouseEvent* event)
{
    /* Fetch the mouse position */
    #if _QT6    /* Qt6 Compat */
    int x = event->position().x();
    int y = event->position().y();
    #else
    int x = event->x();
    int y = event->y();
    #endif // _QT6

    /**
     * If the mouse is currently Pressed
     * track how much we have moved it, this will translate into the panning factor
     * which moves the image around
     */
    if (m_Pressed)
    {
        #if _QT6    /* Qt6 Compat */
        Point delta = event->position() - m_LastMouse;
        /* Update the last mouse pos as well */
        m_LastMouse = event->position();
        #else
        Point delta = event->pos() - m_LastMouse;
        /* Update the last mouse pos as well */
        m_LastMouse = event->pos();
        #endif // _QT6

        /* Calculate the delta for the movement in screen view space */
        float dx = float(delta.x()) / width();
        float dy = float(delta.y()) / height();

        /**
         * Pan speed scales the movement to match the translated OpenGL normalized
         * coordinates which has happened over Screen-space units (or pixels)
         * This speed is also governed by how much we have zoomed in
         */
        float speed = 2.f / m_ZoomFactor;

        /**
         * Open GL expects the 0, 0 of the texture on the bottom left
         * where we're rendering the image upside down to compensate that
         * hence the negated delta y so that the mouse movement correctly corresponds
         * to the movement
         */
        m_Pan += glm::vec2(dx * speed, -dy * speed);

        /* Redraw the image texture, this time with the translation offset applied */
        update();
    }

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

void VoidRenderer::wheelEvent(QWheelEvent* event)
{
    /* Normalize to OpenGL Coords */
    #if _QT6        /* Qt6 Compat */
    m_TranslateX = (2.f * event->position().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->position().y() / height());
    #else
    m_TranslateX = (2.f * event->pos().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->pos().y() / height());
    #endif

    if (event->angleDelta().y() > 0)
    {
        /* +ve or Zoom In */
        m_ZoomFactor *= 1.1f;

        /* The zoom can be upto a max */
        m_ZoomFactor = std::min(m_ZoomFactor, MAX_ZOOM);
    }
    else
    {
        /* Zoom out */
        m_ZoomFactor /= 1.1f;

        /* And upto a minimum */
        m_ZoomFactor = std::max(m_ZoomFactor, MIN_ZOOM);
    }

    VOID_LOG_INFO("Zoom Level: {0}", m_ZoomFactor);

    /* Repaint */
    update();
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
    /* Hide the Error Label */
    m_DisplayLabel->setVisible(false);

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
    /* Hide the Error Label */
    m_DisplayLabel->setVisible(false);

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

void VoidRenderer::ZoomIn(float factor)
{
    m_ZoomFactor *= factor;
    /* Repaint */
    update();
}

void VoidRenderer::ZoomOut(float factor)
{
    m_ZoomFactor *= factor;
    /* Repaint */
    update();
}

void VoidRenderer::ZoomToFit()
{
    /* Reset all attributes contributing to zoom/pan */
    m_ZoomFactor = 1.f;
    m_TranslateX = 0.f;
    m_TranslateY = 0.f;

    /* Reset the panning as well */
    m_Pan = {0.f, 0.f};

    /* Repaint after the zoom attributes have been reset */
    update();
}

void VoidRenderer::UpdateZoom(float zoom)
{
    m_ZoomFactor = zoom;

    /* Repaint */
    update();
}

VOID_NAMESPACE_CLOSE
