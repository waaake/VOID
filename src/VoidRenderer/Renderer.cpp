/* Qt */
#include <QKeyEvent>
#include <QLabel>
#include <QSurfaceFormat>

/* Internal */
#include "Renderer.h"
#include "VoidCore/Logging.h"

static const float MAX_ZOOM = 12.8;
static const float MIN_ZOOM = 0.1;

VOID_NAMESPACE_OPEN

BasicRenderer::BasicRenderer(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_Fullscreen(false)
{
    /* Add Render StatusBar */
    m_RenderStatus = new RendererStatusBar(this);
    /* Message display */
    m_DisplayLabel = new RendererDisplayLabel(this);
    m_DisplayLabel->setVisible(false);

    /* Enable to track mouse movements */
    setMouseTracking(true);
}

BasicRenderer::~BasicRenderer()
{
    /* Delete the Render Status bar */
    m_RenderStatus->deleteLater();
    m_RenderStatus = nullptr;

    /* Delete the Error Label */
    m_DisplayLabel->deleteLater();
    m_DisplayLabel = nullptr;
}

void BasicRenderer::initializeGL()
{
    /* Setup GLEW */
    Setup();

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

    /* Invoke Initialize for child classes to Initialize themselves */
    Initialize();
}

void BasicRenderer::paintGL()
{
    /* Clear Color and Depth */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /**
     * Invoke Draw
     * This is where the drawing happens from the child classes
     */
    Draw();
}

void BasicRenderer::resizeGL(int w, int h)
{
    /* Adjust the viewport size */
    glViewport(0, 0, w, h);
}

void BasicRenderer::resizeEvent(QResizeEvent* event)
{
    /* Base Resize */
    QOpenGLWidget::resizeEvent(event);

    /* The Label has to have a certain gap from the edges */
    m_DisplayLabel->move(10, 10);

    /* Ensure that the status bar always stays at the bottom of the Renderer */
    m_RenderStatus->move(0, height() - m_RenderStatus->height());
    m_RenderStatus->setFixedWidth(width());
}

void BasicRenderer::keyPressEvent(QKeyEvent* event)
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

void BasicRenderer::ClearFrame()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);
}

void BasicRenderer::PrepareFullscreen()
{
    /* Unparent from any current parents */
    setParent(nullptr);

    /* Update the window flags */
    setWindowFlags(Qt::Window);

    /* And update the state to allow the keyboard presses to work as shortcuts */
    m_Fullscreen = true;
}

bool BasicRenderer::Setup()
{
    /* Try and Initialize Glew */
    unsigned int status = glewInit();

    if (status != GLEW_OK)
    {
        VOID_LOG_ERROR("GLEW init Failed: {0}", reinterpret_cast<const char*>(glewGetErrorString(status)));
        return false;
    }

    /* All good for GL and GLEW */
    VOID_LOG_INFO("GLEW Initialized.");
    return true;
}

void BasicRenderer::SetProfile()
{
    /**
     * As we're going to use Modern OpenGL
     * We'd like to use the Core Profile
     * Setup OpenGL Core Profile
     */
    QSurfaceFormat format;
    /**
     * The reason we're going for 3,3 is that whatever we need is available in 3,3 there isn't too much of a benefit
     * going up? maybe we do it later?
     * Plus Apple is kind enough to support only till GL 4.1 so not much to update afterall
     */
    format.setVersion(3, 3);    // This is so that our shader gets compiled and linked version 330 core for OpenGL 3.3
    format.setProfile(QSurfaceFormat::CoreProfile);

    /* Set the adjusted profile */
    QSurfaceFormat::setDefaultFormat(format);

    VOID_LOG_INFO("PROFILE SET....");
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
