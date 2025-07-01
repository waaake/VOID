#ifndef _VOID_RENDERER_H
#define _VOID_RENDERER_H

/* This includes glew which needs to be included before gl.h and hence this gets included first */
#include "VoidGL.h"

/* glm */
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

/* Qt */
#include <QOpenGLWidget>
#include <QImage>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

/* Internal */
#include "PixReader.h"
#include "QDefinition.h"
#include "RendererStatus.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidRenderer : public QOpenGLWidget, protected VoidShader
{
    Q_OBJECT

public: /* Enums */

    enum class ChannelMode : int
    {
        RED,    // Show the Red Channel
        GREEN,  // The Green channel
        BLUE,   // The Blue channel
        ALPHA,  // Alpha Channel
        RGB,    // RGB Channels leaving alpha out
        RGBA    // RGBA or all channels mostly
    };

private: /* Members */
    QOpenGLTexture* m_Texture;
    SharedPixels m_ImageData;
    std::string m_Path;

public:
    VoidRenderer(QWidget* parent = nullptr);

    ~VoidRenderer();

    void Render(SharedPixels data);
    void Play();
    void Clear();

    void ZoomIn(float factor = 1.1f);
    void ZoomOut(float factor = 0.9f);
    void ZoomToFit();
    void UpdateZoom(const float zoom);

    /**
     * Updates necessary attributes to be ready for fullscreen
     */
    void PrepareFullscreen();
    void ExitFullscreen() { m_Fullscreen = false; }
    
    /* Lets other components know whether the Renderer is fullscreen */
    [[nodiscard]] inline bool Fullscreen() const { return m_Fullscreen; }

    /**
     * Adjusts the Exposure of the Viewer
     */
    void SetExposure(const float exposure);
    void SetGamma(const float gamma);
    void SetGain(const float gain);

    /**
     * Set the channels to be displayed on the Viewer
     */
    void SetChannelMode(int mode);

    /*
     * Set a Message to be displayed on the Renderer
     * Mostly gets used to show error messages if anything is not working/available
     */
    inline void SetMessage(const std::string& message)
    {
        /* Update the display label */
        m_DisplayLabel->setText(message.c_str());
        /* And make it visible */
        m_DisplayLabel->setVisible(true);
    }
    
signals: 
    /**
     * Signals controlling the playback for the media
     */
    void playForwards();
    void stop();
    void playBackwards();
    void moveBackward();
    void moveForward();

    /* Exit fullscreen view and back to normal */
    void exitFullscreen();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void keyPressEvent(QKeyEvent* event) override;

    virtual void wheelEvent(QWheelEvent* event) override;

    void ClearFrame();

private: /* Members */
    /** 
     * Array and Buffer objects
     * 
     * Vertex array Object
     * Vertex Buffer Object
     * Element or the index buffer object
     */
    unsigned int VAO, VBO, EBO;

    /**
     * Viewer Adjustments
     * Exposure
     * Gamma
     * Gain
     */
    float m_Exposure, m_Gamma, m_Gain;

    /* Channels to display on the viewport */
    ChannelMode m_ChannelMode;

    RendererStatusBar* m_RenderStatus;
    RendererDisplayLabel* m_DisplayLabel;

    /* Zoom Factor/Level on the Renderer */
    float m_ZoomFactor;
    /* Zoom at specific coords */
    float m_TranslateX, m_TranslateY;

    bool m_Pressed;

    /**
     * Holds the state whether the Renderer is currently rendering
     * fullscreen or in normal view
     */
    bool m_Fullscreen;

    /* Panning */
    glm::vec2 m_Pan;
    Point m_LastMouse;

};

/**
 * A Placeholder Renderer Widget which shows up when the renderer is fullscreen to occupy it's place
 * Holds a Label stating that the Renderer is Fullscreen 
 */
class VOID_API VoidPlaceholderRenderer : public QWidget
{
public:
    VoidPlaceholderRenderer(QWidget* parent = nullptr);

private: /* Members */
    QHBoxLayout* m_Layout;
    QLabel* m_Label;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_H
