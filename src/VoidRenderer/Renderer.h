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
#include "VoidCore/ImageData.h"

VOID_NAMESPACE_OPEN

class VoidRenderer : public QOpenGLWidget, protected QOpenGLFunctions
{
private: /* Members */
    QOpenGLTexture* m_Texture;
    VoidImageData* m_ImageData;
    std::string m_Path;

public:
    VoidRenderer(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
        , m_Texture(nullptr)
        , m_ImageData(nullptr)
    {
    }

    virtual ~VoidRenderer();

    void Render(VoidImageData* data);
    void Play();
    void Clear();
    
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    void ClearFrame();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_H
