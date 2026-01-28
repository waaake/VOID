// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_RENDERER_H
#define _VOID_RENDERER_H

/* This includes glew which needs to be included before gl.h and hence this gets included first */
#include "Renderer.h"

/* glm */
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

/* Internal */
#include "PixReader.h"
#include "Core/RenderTypes.h"
#include "RendererStatus.h"
#include "Layers/ImageRenderLayer.h"
#include "Layers/ImageComparisonRenderLayer.h"
#include "Layers/SwipeRenderLayer.h"
#include "Layers/StrokeRenderLayer.h"
#include "Layers/TextRenderLayer.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidRenderer : public BasicRenderer
{
    Q_OBJECT

private: /* Members */
    /* Render Types */
    using ChannelMode = Renderer::ChannelMode;
    using ComparisonMode = Renderer::ComparisonMode;
    using BlendMode = Renderer::BlendMode;
    using DrawType = Renderer::DrawType;
    using SharedAnnotation = Renderer::SharedAnnotation;

public:
    VoidRenderer(QWidget* parent = nullptr);
    virtual ~VoidRenderer();

    /* Render the Image */
    void Render(SharedPixels data);
    /* Render Image along with Annotations */
    void Render(const SharedPixels& data, const SharedAnnotation& annotation);
    /* Compare 2 Images */
    void Compare(SharedPixels first, SharedPixels second, ComparisonMode comparison, BlendMode blend);
    /* Clears current Frame and rids of any textures that were loaded */
    void Clear();

    /* Set zoom values */
    void ZoomIn(float factor = 1.1f);
    void ZoomOut(float factor = 0.9f);
    void ZoomToFit();
    void UpdateZoom(const float zoom);

    /**
     * Annotation Features
     */
    /* Toggles Annotation state */
    void ToggleAnnotation(bool t);
    [[nodiscard]] inline bool Annotating() const { return m_Annotating; }

    /**
     * Clears any Annotation strokes on the current frame
     */
    void ClearAnnotations();

    /**
     * Sets the color on the annotation -> next stroke gets this color to annotate with
     */
    void SetAnnotationColor(const glm::vec3& color);
    void SetAnnotationColor(const QColor& color);

    /**
     * Sets the size/thickness of the annotation -> next stroke gets the thickness
     */
    void SetAnnotationSize(const float size);

    /**
     * Set the draw type on the annotation Renderer
     */
    void SetAnnotationDrawType(const int type);

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

    /**
     * The color display to be set for the OCIO Color Process
     */
    void SetColorDisplay(const std::string& display);

signals:
    /**
     * Signals for the Annotations
     */
    /* Emitted when a new annotation is just created */
    void annotationCreated(const SharedAnnotation&);
    /* Emitted when annotation on a frame has been cleared/deleted */
    void annotationDeleted();

protected: /* Methods */
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual bool eventFilter(QObject* object, QEvent* event) override;

    /* Draw Functions */
    virtual void Initialize() override;
    virtual void Draw() override;

private: /* Members */
    /* Render Image Data */
    SharedPixels m_ImageA;
    SharedPixels m_ImageB;

    /**
     * Render Layers
     * Renders Textures and other elements on the Viewer
     */
    /* Renders the Main Texture */
    ImageRenderLayer* m_ImageRenderer;
    /* Renders the Textures when the compare mode is set */
    ImageComparisonRenderLayer* m_ImageComparisonRenderer;
    /* Renders all forms of annotations (text | strokes) */
    /* Renders Swipe bar when comparing in Stack */
    SwipeRenderLayer* m_SwipeRenderer;
    StrokeRenderLayer* m_StrokeRenderer;
    TextAnnotationsRenderLayer* m_TextRenderer;

    SharedAnnotation m_Annotation;

    /**
     * ModelViewProjection matrix for the Texture
     */
    glm::mat4 m_VProjection;
    glm::mat4 m_InverseProjection;

    ComparisonMode m_CompareMode;
    BlendMode m_BlendMode;
    DrawType m_DrawType;

    /* The Swipe Controller */
    float m_SwipeX;
    /**
     * When the texture is panned, this controls how much the swipeX needs to be visually
     * panned to maintain evenness with the panned texture
     */
    float m_SwipeOffet;

    bool m_Swiping;

    float m_ZoomFactor;
    float m_TranslateX, m_TranslateY;

    bool m_Pressed;
    bool m_Annotating;
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

    /**
     * @brief Applies inverse Projection tranformation to the Normalized x, y position
     * for the mouse to represent a point accurately in 2D world irrespective of the zoom
     * 
     * The inverse of the world transform (model view projection) when going back from 
     * screen space to logical coordinate space (annotation position)
     * Takes in the Normalized Device Converted Coordinates from the Mouse.x Mouse.y -> Normalized to [-1, 1] Gl Viewport
     * Applies the inverse of the projection matrix to the 4 vec of (x, y, z, w)
     * where w is 1 (homogeneous Coordinate) cannot be 0 else the value gets treated as a direction vector 
     * z can be 0 as for 2D the depth doesn't matter
     * 
     * @param normalized Normalized Device Converted Coordinates from the Mouse.x Mouse.y.
     */
    inline glm::vec2 InverseWorldPoint(const glm::vec2& normalized) const
    {
        glm::vec4 worldCoord = m_InverseProjection * glm::vec4(normalized.x, normalized.y, 0.f, 1.f);
        return glm::vec2(worldCoord.x, worldCoord.y);
    }

    /**
     * @brief Loads the Textures With Image Data
     * 
     */
    void ReloadTextures();

    /**
     * @brief (Re)Sets the Mouse pointer based on the current Annotation tool
     * 
     */
    void ResetAnnotationPointer();

    /**
     * @brief Handles the KeyPresses for Annotation and converts the required keys into text which is rendered
     * on the viewport, handles other key presses like Escape to exit out from the mode etc.
     * 
     */
    void HandleAnnotationTyping(QKeyEvent* event);

    /**
     * @brief Checks with the AnnotationRenderLayer to see if an annotation is currently active
     * if an annotation is active, all good -- that'll be used for drawing onto
     * if an annotation isn't present or has been deleted, then a new annotation will be created
     * and the annotationCreated signal will be invoked for any other component to handle annotation
     * that has been added
     * 
     */
    void EnsureHasAnnotation();

    /**
     * @brief Set the Annotation data for the Renderer.
     * 
     * @param annotation Annotation data to be set/rendered for the frame.
     */
    void SetAnnotation(const Renderer::SharedAnnotation& annotation);

    /**
     * @brief Clears the underlying annotation data from the Renderer.
     * 
     */
    void RemoveAnnotation();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_H
