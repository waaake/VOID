#ifndef _VOID_RENDERER_H
#define _VOID_RENDERER_H

/* Qt */
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QImage>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

/* Internal */
#include "Definition.h"
#include "RendererStatus.h"
#include "VoidCore/ImageData.h"

VOID_NAMESPACE_OPEN

class VoidRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
private: /* Members */
    QOpenGLTexture* m_Texture;
    VoidImageData* m_ImageData;
    std::string m_Path;

public:
    VoidRenderer(QWidget* parent = nullptr);

    virtual ~VoidRenderer();

    void Render(VoidImageData* data);
    void Play();
    void Clear();

    void ZoomIn(float factor = 1.1f);
    void ZoomOut(float factor = 0.9f);
    void ZoomToFit();
    void UpdateZoom(float zoom);
    
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

    virtual void wheelEvent(QWheelEvent* event) override;

    void ClearFrame();

private: /* Members */
    RendererStatusBar* m_RenderStatus;

    /* Zoom Factor/Level on the Renderer */
    float m_ZoomFactor;
    /* Zoom at specific coords */
    float m_TranslateX, m_TranslateY;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_H
