#ifndef _VOID_RENDERER_H
#define _VOID_RENDERER_H

/* This includes glew which needs to be included before gl.h and hence this gets included first */
#include "VoidGL.h"

/* STD */
#include <vector>

/* glm */
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

/* Qt */
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

/* Internal */
#include "PixReader.h"
#include "QDefinition.h"
#include "RenderTypes.h"
#include "RendererStatus.h"
#include "AnnotationRenderer.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidRenderer : public QOpenGLWidget, protected VoidShader
{
    Q_OBJECT

private: /* Members */
    QOpenGLTexture* m_Texture;
    SharedPixels m_ImageA;
    SharedPixels m_ImageB;
    std::string m_Path;

    /* Enum Types */
    using ChannelMode = Renderer::ChannelMode;
    using ComparisonMode = Renderer::ComparisonMode;
    using BlendMode = Renderer::BlendMode;

public:
    VoidRenderer(QWidget* parent = nullptr);

    ~VoidRenderer();

    void Render(SharedPixels data);
    void Compare(SharedPixels first, SharedPixels second, ComparisonMode comparison, BlendMode blend);
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

    /**
     * Annotation Features
     */
    /* Toggles Annotation state */
    inline void ToggleAnnotation(bool t) { m_Annotating = t; }
    [[nodiscard]] inline bool Annotating() const { return m_Annotating; }

    /**
     * Clears any Annotation strokes on the current frame
     */
    inline void ClearAnnotations() { m_AnnotationsRenderer->Clear(); update(); }

    /**
     * Sets the color on the annotation -> next stroke gets this color to annotate with
     */
    inline void SetAnnotationColor(const glm::vec3& color) { m_AnnotationsRenderer->SetColor(color); }
    inline void SetAnnotationColor(const QColor& color) { m_AnnotationsRenderer->SetColor(color); }

    /**
     * Sets the thickness of the annotation -> next stroke gets the thickness
     */
    inline void SetAnnotationBrushSize(const float thickness) { m_AnnotationsRenderer->SetAnnotationBrushSize(thickness); }

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

    // /**
    //  * Setup the Comparisons
    //  */
    // void SetComparisonMode(ComparisonMode mode);
    // void SetBlendMode(BlendMode mode);

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
     * Array and Buffer objects for the swipe handle
     */
    unsigned int m_SwipeVAO, m_SwipeVBO;

    /**
     * Renders All kinds of annotations
     */
    VoidAnnotationsRenderer* m_AnnotationsRenderer;

    /**
     * Viewer Adjustments
     * Exposure
     * Gamma
     * Gain
     */
    float m_Exposure, m_Gamma, m_Gain;

    /**
     * Render Textures
     */
    unsigned int m_TextureA, m_TextureB;

    /**
     * ModelViewProjection matrix for the Texture
     */
    glm::mat4 m_ModelViewProjection;

    /* Channels to display on the viewport */
    ChannelMode m_ChannelMode;

    /* Comparison Mode for the buffers */
    ComparisonMode m_CompareMode;
    /* Blend Mode for the comparison */
    BlendMode m_BlendMode;

    RendererStatusBar* m_RenderStatus;
    RendererDisplayLabel* m_DisplayLabel;

    /* The Swipe Controller */
    float m_SwipeX;
    /**
     * When the texture is panned, this controls how much the swipeX needs to be visually
     * panned to maintain evenness with the panned texture
     */
    float m_SwipeOffet;

    bool m_Swiping;

    /* Zoom Factor/Level on the Renderer */
    float m_ZoomFactor;
    /* Zoom at specific coords */
    float m_TranslateX, m_TranslateY;

    bool m_Pressed;

    /**
     * State describing if the Renderer is currently up for annotation
     */
    bool m_Annotating;

    /**
     * Holds the state whether the Renderer is currently rendering
     * fullscreen or in normal view
     */
    bool m_Fullscreen;

    /* Panning */
    glm::vec2 m_Pan;
    Point m_LastMouse;

private: /* Methods */
    /**
     * Based on the Comparison Mode this returns the factor with which the width of the view is divided
     * to get the correct aspect for the Rendering of the texture
     * This is needed for Horizontal and Vertical Comparisons to retain the aspect when two (or more) textures
     * are simultaneously played
     */
    inline float WidthDivisor() const
    {
        return (m_CompareMode == ComparisonMode::HORIZONTAL) ? 2.f : 1.f;
    }
    inline float HeightDivisor() const
    {
        return (m_CompareMode == ComparisonMode::VERTICAL) ? 2.f : 1.f;
    }

    /**
     * Redraw after calculating the Projection Matrix
     */
    inline void Redraw()
    {
        CalculateModelViewProjection();
        update(); // Redraw
    }

    /**
     * Calculates the ModelViewProjection Matrix for the Image Texture
     */
    void CalculateModelViewProjection();
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
