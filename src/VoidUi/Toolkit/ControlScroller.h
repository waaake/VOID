// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_CONTROL_SCROLLER_H
#define _VOID_CONTROL_SCROLLER_H

/* STD */
#include <vector>

/* Qt */
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
#include <QWidget>

/* Internal */
#include "QDefinition.h"
#include "VoidUi/QExtensions/Label.h"
#include "VoidUi/QExtensions/SpinBox.h"

VOID_NAMESPACE_OPEN

class ControlCombo : public QComboBox
{
public:
    ControlCombo(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

class ControlSpinner : public QDoubleSpinBox
{
public:
    ControlSpinner(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    /* Setup the Cursor Override */
    void enterEvent(EnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private: /* Members */
    /* Denotes the Last Mouse X Position */
    int m_LastX;

    /* Scroll threshold */
    int m_Threshold;

};

class QuickSpinner : public QWidget
{
    Q_OBJECT

public:
    QuickSpinner(QWidget* parent = nullptr);
    QuickSpinner(const std::string& label, const double min, const double max, const double value, QWidget* parent = nullptr);
    ~QuickSpinner();

    /* Range Adjustments */
    inline void SetMinimum(const double min) { m_Spinner->setMinimum(min); }
    inline void SetMaximum(const double max) { m_Spinner->setMaximum(max); }
    inline void SetRange(const double min, const double max) { m_Spinner->setRange(min, max); }
    inline void SetStep(const double step) { m_Spinner->setSingleStep(step); }

    /* Set Current Value */
    inline void SetValue(const double value) { m_Spinner->setValue(value); }

    /* Set Default value */
    inline void SetDefault(const double value) { m_Default = value; }

    /* Add Label for the Adjustment */
    inline void SetLabel(const std::string& text) { m_Label->setText(text.c_str()); }

signals:
    void valueChanged(const double);

private: /* Members */
    /* Base Layout */
    QHBoxLayout* m_Layout;

    ClickableLabel* m_Label;
    ControlDoubleSpinner* m_Spinner;

    double m_Default;
    double m_Last;

private: /* Methods */
    /* Build UI */
    void Build();

    /* What happens when the value is changed */
    void ValueChanged(const double value);

    /* Reset Value to default or the last */
    void Reset();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_CONTROL_SCROLLER_H
