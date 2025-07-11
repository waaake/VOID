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
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

class ControlCombo : public QComboBox
{
public:
    ControlCombo(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

class ControlSpinner : public QSpinBox
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

class ControlDoubleSpinner : public QDoubleSpinBox
{
public:
    ControlDoubleSpinner(QWidget* parent = nullptr);

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

VOID_NAMESPACE_CLOSE

#endif // _VOID_CONTROL_SCROLLER_H
