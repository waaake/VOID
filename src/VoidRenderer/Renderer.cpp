/* System */
// #include <GL/gl.h>

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>     // for ortho/scale
#include <glm/gtc/type_ptr.hpp>             // for glm::value_ptr

/* Qt */
#include <QKeyEvent>
#include <QLabel>
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
    , m_ImageA(nullptr)
    , m_ImageB(nullptr)
    , m_Exposure(0.f)
    , m_Gamma(1.f)
    , m_Gain(1.f)
    , m_TextureA(0)
    , m_TextureB(0)
    , m_ChannelMode(ChannelMode::RGBA)
    , m_CompareMode(ComparisonMode::NONE)
    , m_BlendMode(BlendMode::UNDER)
    , m_SwipeX(0.5f)
    , m_SwipeOffet(0.f)
    , m_Swiping(false)
    , m_ZoomFactor(1.f)
    , m_TranslateX(0.f)
    , m_TranslateY(0.f)
    , m_Pressed(false)
    , m_Fullscreen(false)
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

    /* Destroy texture */
    glDeleteTextures(1, &m_TextureA);
    glDeleteTextures(1, &m_TextureB);

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

    /* Generate Textures for Rendering */
    glGenTextures(1, &m_TextureA);
    glGenTextures(1, &m_TextureB);

    /**
     * Swipe Shaders
     */
    float swipeVertices[6] = {
        0.f, -1.f, 0.f,
        0.f,  1.f, 0.f
    };

    glGenVertexArrays(1, &m_SwipeVAO);
    glGenBuffers(1, &m_SwipeVBO);

    /* Bind */
    glBindVertexArray(m_SwipeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SwipeVBO);

    /* Buffer data */
    glBufferData(GL_ARRAY_BUFFER, sizeof(swipeVertices), swipeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VoidRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_ImageA && !m_ImageA->Empty())
    {
        /* Use the Main Image Render Program */
        glUseProgram(ProgramId());

        /**
         * Bind the shader program to use for
         */
        Bind();

        glActiveTexture(GL_TEXTURE0);
        /* Bind the Generated texture for Render */
        glBindTexture(GL_TEXTURE_2D, m_TextureA);

        /* Tell the shader what texture to use */
        glUniform1i(glGetUniformLocation(ProgramId(), "uTexture"), 0);

        /* Bind the texture for render */
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_TextureB);

        /* Tell this to the shader to use this texture as the second sampler2D */
        glUniform1i(glGetUniformLocation(ProgramId(), "uTextureB"), 1);

        CalculateModelViewProjection();

        GLuint projLoc = glGetUniformLocation(ProgramId(), "uMVP");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_ModelViewProjection));

        /* Update the viewer properties to the shader */
        SetUniform("exposure", m_Exposure);
        SetUniform("gamma", m_Gamma);
        /**
         * Gain is the linear multiplier to amplify brigtness
         * Can be used befor or after exposure or gamma correction
         */
        SetUniform("gain", m_Gain);

        /**
         * Update the channels to be displayed on the renderer
         */
        SetUniform("channelMode", static_cast<int>(m_ChannelMode));

        /**
         * Update the compare mode on the shader
         */
        SetUniform("comparisonMode", static_cast<int>(m_CompareMode));

        /**
         * Update the blend mode on the shader
         */
        SetUniform("blendMode", static_cast<int>(m_BlendMode));

        /**
         * Update the swipe factor
         */
        SetUniform("swipeX", m_SwipeX);

        /* And the Vertex Array */
        glBindVertexArray(VAO);

        /**
         * Draw triangles as bound in the Index buffer as defined earlier
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

        /* Release the active shader program from the current OpenGL Context */
        Release();

        /* Draw the Swipe Controller if */
        if (m_CompareMode == ComparisonMode::WIPE)
        {
            /* Swipe Lines */
            BindSwiper();

            /**
             * Normalize between the device coordinates
             * the SwipeX limit is between 0.0 - 1.0
             * to make it work in OpenGL's Normalized coordinates -1.0 - 1.0
             * the math needs to be applied
             *
             * lowest: (0.f * 2.f) - 1.f = -1.f;
             * mid: (0.5f * 2.f) - 1.f = 0.f;
             * highest: (1.f * 2.f) - 1.f = 1.f;
             */
            float swiperNormalized = ((m_SwipeX + m_SwipeOffet) * 2.f) - 1.f;

            /* Model view projection for the swiper */
            glm::mat4 swproj = glm::ortho(-1.f, 1.f, -1.f, 1.f);
            glm::mat4 swmodel = glm::translate(glm::mat4(1.f), glm::vec3(swiperNormalized, 0.f, 0.f));
            glm::mat4 swiperMvp = swproj * swmodel;

            /* Use the Swipe Shader */
            glUseProgram(SwipeProgramId());
            /* Set Model View Projection Matrix */
            glUniformMatrix4fv(glGetUniformLocation(SwipeProgramId(), "uMVP"), 1, GL_FALSE, glm::value_ptr(swiperMvp));
            /* Set the Color */
            float color[3] = {1.f, 1.f, 1.f};
            glUniform3fv(glGetUniformLocation(SwipeProgramId(), "uColor"), 1, color);

            /* Bind the Array */
            glBindVertexArray(m_SwipeVAO);
            glDrawArrays(GL_LINES, 0, 2);

            /* Unbind */
            glBindVertexArray(0);

            ReleaseSwiper();
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

    /* The Swipe only works when we're in Wipe mode for Comparison -- else the Swiper isn't shown */
    if (m_CompareMode == ComparisonMode::WIPE)
    {
        /* Convert the Swipe onto Qt Coordinates actual window width */
        float swipex = (m_SwipeX + m_SwipeOffet) * width();

        /* A Buffer of 10 pixels to allow swiping */
        if (std::abs(m_LastMouse.x() - swipex) < 10.f)
        {
            /* Update to indicate that we're now swiping and not panning */
            m_Swiping = true;
        }
    }
}

void VoidRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    /* And indicates that the mouse was released */
    m_Pressed = false;
    m_Swiping = false;
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

    /* The Swipe only works when we're in Wipe mode for Comparison -- else the Swiper isn't shown */
    if (m_CompareMode == ComparisonMode::WIPE)
    {
        /* Convert the Swipe onto Qt Coordinates actual window width */
        float swipex = (m_SwipeX + m_SwipeOffet) * width();

        /* A Buffer of 10 pixels to allow swiping */
        if (std::abs(x - swipex) < 10.f)
        {
            /* Update the Mouse Cursor */
            setCursor(Qt::SizeHorCursor);
        }
        else
        {
            /* Restore the cursor */
            unsetCursor();
        }

        /* Check if we're Swiping */
        if (m_Swiping)
        {
            /* Update the Swipe in it's normalized system (0.f - +1.f)*/
            m_SwipeX = std::clamp(x / float(width()), 0.f, 1.f);
    
            /* Redraw the Texture */
            update();
    
            /* Return from here and not affect the pan */
            return;
        }
    }


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
         * Update the offset with which the swipe handle needs to be moved to maintain a 1:1
         * placement with the panned texture
         */
        m_SwipeOffet += dx;

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
    #if _QT6_COMPAT        /* Qt6 Compat */
    m_TranslateX = (2.f * event->position().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->position().y() / height());
    #else
    m_TranslateX = (2.f * event->pos().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->pos().y() / height());
    #endif // Qt6 Compat

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

void VoidRenderer::keyPressEvent(QKeyEvent* event)
{
    /* The below keypresses only work when we're fullscreen */
    if (m_Fullscreen)
    {
        switch (event->key())
        {
            case Qt::Key_Escape:
                emit exitFullscreen();
                break;
            case Qt::Key_Period:
                emit moveForward();
                break;
            case Qt::Key_Comma:
                emit moveBackward();
                break;
            case Qt::Key_J:
                emit playBackwards();
                break;
            case Qt::Key_K:
                emit stop();
                break;
            case Qt::Key_L:
                emit playForwards();
                break;
        }
    }

    /* Base Key presses */
    QOpenGLWidget::keyPressEvent(event);
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

void VoidRenderer::Render(SharedPixels data)
{
    /* Update the image data */
    m_ImageA = data;
    /* Hide the Error Label */
    m_DisplayLabel->setVisible(false);

    /* We're no longer comparing */
    m_CompareMode = ComparisonMode::NONE;

    /* Clear Secondary Image Data*/
    m_ImageB = nullptr;

    /**
     * Update the render resolution
     * The resolution of the texture is not going to change in the draw (unless we apply a reformat to it)
     * So constantly redrawing this is just too ineffecient
     */
    if (m_ImageA)
    {
        m_RenderStatus->SetRenderResolution(m_ImageA->Width(), m_ImageA->Height());

        /* Bind the Generated texture for Render */
        glBindTexture(GL_TEXTURE_2D, m_TextureA);
        /**
         * Load the image data onto the Texture 2D
         */
        glTexImage2D(GL_TEXTURE_2D, 0, m_ImageA->GLFormat(), m_ImageA->Width(), m_ImageA->Height(), 0, m_ImageA->GLFormat(), m_ImageA->GLType(), m_ImageA->Pixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Compare(SharedPixels first, SharedPixels second, ComparisonMode comparison, BlendMode blend)
{
    /* Update the image data */
    m_ImageA = first;
    m_ImageB = second;

    /* Update the Comparison Mode */
    m_CompareMode = comparison;
    /* Update the Blend Mode */
    m_BlendMode = blend;

    /* Hide the Error Label */
    m_DisplayLabel->setVisible(false);

    /**
     * Update the render resolution
     * The resolution of the texture is not going to change in the draw (unless we apply a reformat to it)
     * So constantly redrawing this is just too ineffecient
     */
    if (m_ImageA)
    {
        m_RenderStatus->SetRenderResolution(m_ImageA->Width(), m_ImageA->Height());

        /* Bind the Generated texture for Render */
        glBindTexture(GL_TEXTURE_2D, m_TextureA);
        /**
         * Load the image data onto the Texture 2D
         */
        glTexImage2D(GL_TEXTURE_2D, 0, m_ImageA->GLFormat(), m_ImageA->Width(), m_ImageA->Height(), 0, m_ImageA->GLFormat(), m_ImageA->GLType(), m_ImageA->Pixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    if (m_ImageB)
    {
        /* Bind the texture for render */
        glBindTexture(GL_TEXTURE_2D, m_TextureB);

        /**
         * Load the image data onto the Texture 2D
         */
        glTexImage2D(GL_TEXTURE_2D, 0, m_ImageB->GLFormat(), m_ImageB->Width(), m_ImageB->Height(), 0, m_ImageB->GLFormat(), m_ImageB->GLType(), m_ImageB->Pixels());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   
    }

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Play()
{

}

void VoidRenderer::Clear()
{
    /* Delete the reference to the Image Data */
    m_ImageA = nullptr;
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

    /* Reset the SwipeOffset to make it come to it's original place */
    m_SwipeOffet = 0.f;

    /* Repaint after the zoom attributes have been reset */
    update();
}

void VoidRenderer::UpdateZoom(const float zoom)
{
    m_ZoomFactor = zoom;

    /* Repaint */
    update();
}

void VoidRenderer::SetExposure(const float exposure)
{
    m_Exposure = exposure;
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetGamma(const float gamma)
{
    m_Gamma = gamma;
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetGain(const float gain)
{
    m_Gain = gain;
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetChannelMode(int mode)
{
    /* Update the channel mode for the Renderer */
    m_ChannelMode = static_cast<ChannelMode>(mode);

    /* Redraw the texture */
    update();
}

void VoidRenderer::PrepareFullscreen()
{
    /* Unparent from any current parents */
    setParent(nullptr);

    /* Update the window flags */
    setWindowFlags(Qt::Window);

    /* And update the state to allow the keyboard presses to work as shortcuts */
    m_Fullscreen = true;
}

void VoidRenderer::CalculateModelViewProjection()
{
    /**
     * No Image Data to Proceed
     */
    if (!m_ImageA || m_ImageA->Empty())
        return;

    /**
     * To ensure the image is of the correct aspect while render
     * Calculate the aspect of the current view (Renderer Width / Renderer Height)
     * And the aspect of the image being rendered
     */
    float viewAspect = (width() / WidthDivisor()) / (height() / HeightDivisor());
    float imageAspect = float(m_ImageA->Width()) / float((m_ImageA->Height() ? m_ImageA->Height() : 1));

    /* Find the overall scale of the image */
    glm::vec2 scale = (imageAspect > viewAspect) ? glm::vec2(1.f, viewAspect / imageAspect) : glm::vec2(imageAspect / viewAspect, 1.f);

    /**
     * Get the Model matrix,
     * This is how our image/model looks like as a 4x4 matrix
     */
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
    m_ModelViewProjection = model * projection; // * unity view i.e. glm::mat4(1.f)    
}

/* Placeholder Renderer {{{ */

VoidPlaceholderRenderer::VoidPlaceholderRenderer(QWidget* parent)
    : QWidget(parent)
{
    /* Setup */
    m_Layout = new QHBoxLayout(this);
    /* Label description */
    m_Label = new QLabel("The Renderer is currently playing in Fullscreen View.");

    /* Update label font */
    QFont f = m_Label->font();
    f.setPixelSize(24);
    m_Label->setFont(f);

    /* Add to the layout */
    m_Layout->addWidget(m_Label, 1, Qt::AlignCenter);
}

/* }}} */

VOID_NAMESPACE_CLOSE
