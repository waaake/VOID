// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_SLIDER_H
#define _VOID_Q_EXT_SLIDER_H

/* Qt */
#include <QWidget>
#include <QLineEdit>
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

class SimpleSlider : public QSlider
{
public:
    SimpleSlider(Qt::Orientation orientation, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};

class QuickDoubleSlider : public QWidget
{
    Q_OBJECT
public:
    QuickDoubleSlider(QWidget* parent = nullptr);
    ~QuickDoubleSlider();

    void SetValue(double value);
    void SetMinimum(double min);
    void SetMaximum(double max);
    void SetRange(double min, double max);
    void SetSingleStep(double step);

    double Value() const { return m_Editor->text().toDouble(); }

signals:
    void valueChanged(double);

private: /* Members */
    QHBoxLayout* m_Layout;
    QLineEdit* m_Editor;
    SimpleSlider* m_Slider;

    int m_Min;
    int m_Max;
    unsigned int m_Factor;

private: /* Methods */
    void Build();
    void Setup();
    void EditorUpdated();
    void CalculateFactor(double minimum);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_SLIDER_H
