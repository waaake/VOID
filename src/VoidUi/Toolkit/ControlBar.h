// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_VIEWER_CONTROL_BAR_H
#define _VOID_VIEWER_CONTROL_BAR_H

/* Qt */
#include <QComboBox>
#include <QLayout>
#include <QSpinBox>
#include <QWidget>

/* Internal */
#include "BufferSwitch.h"
#include "ControlScroller.h"
#include "ColorController.h"
#include "Definition.h"
#include "VoidUi/QExtensions/PushButton.h"

VOID_NAMESPACE_OPEN

class ControlSlider : public QSlider
{
    Q_OBJECT

public:
    ControlSlider(Qt::Orientation orientation, QWidget* parent = nullptr);
    virtual ~ControlSlider();

protected:
    void paintEvent(QPaintEvent* event) override;

};

class ControlBar : public QWidget
{
    Q_OBJECT

public:
    ControlBar(ViewerBuffer* A, ViewerBuffer* B, QWidget* parent = nullptr);
    virtual ~ControlBar();

    void SetZoom(float zoom);
    inline void ZoomIn() { m_Zoomer->setValue(m_Zoomer->value() + 10); }
    inline void ZoomOut() { m_Zoomer->setValue(m_Zoomer->value() - 10); }
    void SetZoomLimits(float min, float max);

    /**
     * Sets the current Compare mode
     */
    inline void SetCompareMode(const Renderer::ComparisonMode& mode) { m_BufferSwitch->SetCompareMode(mode); }

signals:
    void zoomChanged(const float factor);
    void exposureChanged(const float exposure);
    void gammaChanged(const float gamma);
    void gainChanged(const float gain);
    void viewerBufferSwitched(const PlayerViewBuffer&);
    void channelModeChanged(const int);
    void comparisonModeChanged(const int);
    void blendModeChanged(const int);
    void annotationsToggled(const int);
    void colorDisplayChanged(const std::string&);

private: /* Members */
    /* Base Layout */
    QHBoxLayout* m_Layout;

    /**
     * Viewer Properties
     * Exposure
     * Gamma
     * Gain
     */
    QuickSpinner* m_ExposureSpinner;
    QuickSpinner* m_GammaSpinner;
    QuickSpinner* m_GainSpinner;

    /**
     * Set/Update channels on the viewer
     */
    ControlCombo* m_ChannelModeController;

    /* Zoom Controls */
    ControlSpinner* m_Zoomer;

    /* The current Dsiplays to be selected for Color */
    ColorController* m_ColorDisplayController;

    /* Annotation Controller */
    HighlightToggleButton* m_AnnotationButton;

    /* Viewer Buffers */
    ViewerBuffer* m_ViewerBufferA;
    ViewerBuffer* m_ViewerBufferB;
    
    /* Viewer Buffer Switch */
    BufferSwitch* m_BufferSwitch;

    /**
     * In order to keep the ViewerBuffer selector always in the center,
     * the best approach is just to use left and right widgets with spacer internally
     */
    QWidget* m_LeftControls;
    QWidget* m_RightControls;

    QHBoxLayout* m_LeftLayout;
    QHBoxLayout* m_RightLayout;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();
    QFrame* Separator();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_CONTROL_BAR_H
