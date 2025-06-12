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
#include "Definition.h"
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
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent* event) override;
    #else
    void enterEvent(QEvent* event) override;
    #endif
    void leaveEvent(QEvent* event) override;


private: /* Members */
    /* Denotes the Last Mouse X Position */
    int m_LastX;

    /* Scroll threshold */
    int m_Threshold;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_CONTROL_SCROLLER_H
