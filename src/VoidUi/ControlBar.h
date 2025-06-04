#ifndef _VOID_VIEWER_CONTROL_BAR_H
#define _VOID_VIEWER_CONTROL_BAR_H

/* Qt */
#include <QComboBox>
#include <QLayout>
#include <QSpinBox>
#include <QWidget>

/* Internal */
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
    ControlBar(QWidget* parent = nullptr);
    virtual ~ControlBar();

    /*
     * Since the viewport could also have a zoom applied on itself, that value should then also
     * need to reflect on the zoom slider, and hence the zoom factor needs to be translated to Slider
     * space and then set on the Zoom slider
     * Also since the slider is connected to emit zoomChanged with value changes,
     * the signals are blocked till the time the value has been set on the slider and unblocked after
     */
    void SetFromZoom(float zoom);

signals:
    void zoomChanged(float factor);
    void missingFrameHandlerChanged(int handler);

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    /* Base Layout */
    QHBoxLayout* m_Layout;

    /* Zoom Controls */
    ControlSpinner* m_Zoomer;

    /* Missing Frame Handler */
    /* TODO: This is temporarily here till we have a place for settings/preferences defined */
    QLabel* m_MissingFrameLabel;
    ControlCombo* m_MissingFrameCombo;
    QHBoxLayout* m_MissingFrameLayout;

private: /* Methods */
    void Build();
    void Setup();
    void Connect();

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
