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
#include "Definition.h"
#include "VoidCore/Logging.h"

/* Describes the amount of zoom (in or out) that happens when the zoom buttons are pressed */
static const int ZOOM_STEP = 10;

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

    /**
     * Since the viewport could also have a zoom applied on itself, that value should then also
     * need to reflect on the zoom slider, and hence the zoom factor needs to be translated to Slider
     * space and then set on the Zoom slider
     * Also since the slider is connected to emit zoomChanged with value changes,
     * the signals are blocked till the time the value has been set on the slider and unblocked after
     */
    void SetFromZoom(float zoom);

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

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    /* Base Layout */
    QHBoxLayout* m_Layout;

    /**
     * Viewer Properties
     * Exposure
     * Gamma
     * Gain
     */
    QLabel* m_ExposureLabel;
    ControlDoubleSpinner* m_ExposureController;
    QLabel* m_GammaLabel;
    ControlDoubleSpinner* m_GammaController;
    QLabel* m_GainLabel;
    ControlDoubleSpinner* m_GainController;

    /**
     * Set/Update channels on the viewer
     */
    ControlCombo* m_ChannelModeController;

    /* Zoom Controls */
    ControlSpinner* m_Zoomer;

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

    /**
     * Returns a Vertical Separator to be added to the Layout
     * This separator is a way to separate Tool controllers
     */
    QFrame* Separator();

    /* Zoom Updates */
    inline void ZoomIn() { m_Zoomer->setValue(m_Zoomer->value() + ZOOM_STEP); }
    inline void ZoomOut() { m_Zoomer->setValue(m_Zoomer->value() - ZOOM_STEP); }

    /* 
     * Emits the zoomChanged signal with the value from zoom slider normalized to viewport zoom
     */
    inline void UpdateZoom(int value) { emit zoomChanged(MapToZoom(value)); }
    // inline void UpdateZoom(int zoom) { VOID_LOG_INFO("Z: {0}", MapToZoom(zoom)); }

    float MapToZoom(int value);
    int MapFromZoom(float zoom);

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_VIEWER_CONTROL_BAR_H
