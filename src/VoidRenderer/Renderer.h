// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_RENDERER_H
#define _VOID_BASIC_RENDERER_H

/* GLEW */
#include <GL/glew.h>

/* Qt */
#include <QOpenGLWidget>

/* Internal */
#include "QDefinition.h"
#include "RendererStatus.h"

VOID_NAMESPACE_OPEN

class VOID_API BasicRenderer : public QOpenGLWidget
{
    Q_OBJECT

public:
    BasicRenderer(QWidget* parent = nullptr);
    virtual ~BasicRenderer();

    /**
     * Updates necessary attributes to be ready for fullscreen
     */
    void PrepareFullscreen();
    void ExitFullscreen() { m_Fullscreen = false; }

    /* Lets other components know whether the Renderer is fullscreen */
    [[nodiscard]] inline bool Fullscreen() const { return m_Fullscreen; }

    /*
     * Set a Message to be displayed on the Renderer
     * Mostly gets used to show error messages if anything is not working/available
     */
    inline void SetMessage(const std::string& message)
    {
        /* Update the display label */
        m_DisplayLabel->setText(message.c_str());
        /* And set it's visibility based on the message */
        m_DisplayLabel->setVisible(!message.empty());
    }

    /**
     * Sets Core Profile for OpenGL
     * This needs to be invoked before the context is initialized
     * That means it should get invoked before we create the instance of the VoidRenderer.
     */
    static void SetProfile();

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

protected: /* Members */
    /* Indicates whether the Renderer is in fullscreen view */
    bool m_Fullscreen;

    RendererStatusBar* m_RenderStatus;
    RendererDisplayLabel* m_DisplayLabel;

private: /* Methods */
    /* GL Functions */
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

protected: /* Methods */
    /* Clears Frame */
    void ClearFrame();

    /* This is called after the context is setup and Glew is Initialized */
    virtual void Initialize() = 0;

    /* Any render on the child class should happen Here */
    virtual void Draw() = 0;

    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private: /* Methods */
    /**
     * Sets up Glew
     */
    bool Setup();

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

#endif // _VOID_BASIC_RENDERER_H
