#ifndef _VOID_Q_EXT_SLIDER_H
#define _VOID_Q_EXT_SLIDER_H

/* Qt */
#include <QWidget>
#include <QSlider>
#include <QLayout>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class FramelessSlider : public QWidget
{
    Q_OBJECT

public:
    FramelessSlider(QWidget* parent);
    ~FramelessSlider();

    /**
     * Setup Range for the slider
     */
    void SetMinimum(const int value) { m_Slider->setMinimum(value); }
    void SetMaximum(const int value) { m_Slider->setMaximum(value); }
    void SetValue(const int value) { m_Slider->setValue(value); }

signals:
    void valueChanged(const int);

private: /* Members */
    QHBoxLayout* m_Layout;
    QSlider* m_Slider;

private: /* Methods */
    void Build();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_SLIDER_H
