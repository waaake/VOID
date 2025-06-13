/* Qt */
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>

/* Internal */
#include "ControlScroller.h"

VOID_NAMESPACE_OPEN

/* Control Combo {{{ */

ControlCombo::ControlCombo(QWidget* parent)
    : QComboBox(parent)
{
    /* Set Focus to never stay on it */
    setFocusPolicy(Qt::NoFocus);
}

void ControlCombo::paintEvent(QPaintEvent* event)
{
    /* Painter to draw elements */
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    /* Text */
    painter.setPen(QColor(210, 210, 210));
    painter.drawText(rect().adjusted(5, 0, -5, 0), Qt::AlignLeft | Qt::AlignVCenter, currentText());

    /* Arrow Indicator */
    QPoint arrowCenter(width() - 15, height() / 2);
    QPolygon downarrow;

    /* Add Points */
    downarrow << QPoint(arrowCenter.x() - 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x() + 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x(), arrowCenter.y() + 4);

    /* Draw the Arrow polygon */
    painter.setBrush(QColor(210, 210, 210));
    painter.drawPolygon(downarrow);
}

/* }}} */

/* Control Spinner {{{ */

ControlSpinner::ControlSpinner(QWidget* parent)
    : QSpinBox(parent)
    , m_LastX(0)
    , m_Threshold(10)
{
    /* Use Simple arrows */
    setButtonSymbols(QAbstractSpinBox::PlusMinus);

    /* Hide the text input */
    lineEdit()->setVisible(false);

    /* Set Focus to never stay on it */
    setFocusPolicy(Qt::NoFocus);
}

void ControlSpinner::paintEvent(QPaintEvent* event)
{
    /* Painter to draw elements */
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    /* Text */
    painter.setPen(QColor(210, 210, 210));
    painter.drawText(rect().adjusted(0, 0, -15, 0), Qt::AlignCenter, (QString::number(value()) + " %"));

    /* Draw Arrows */
    QPoint arrowCenter(width() - 8, height() / 2);
    QPolygon downArrow;
    QPolygon upArrow;

    /* Add Points */
    downArrow << QPoint(arrowCenter.x() - 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x() + 3, arrowCenter.y() + 1)
        << QPoint(arrowCenter.x(), arrowCenter.y() + 5);
    
    upArrow << QPoint(arrowCenter.x() - 3, arrowCenter.y() - 1)
        << QPoint(arrowCenter.x() + 3, arrowCenter.y() - 1)
        << QPoint(arrowCenter.x(), arrowCenter.y() - 5);

    /* Draw the Arrow polygon */
    painter.setBrush(QColor(210, 210, 210));
    painter.drawPolygon(upArrow);
    painter.drawPolygon(downArrow);
}

void ControlSpinner::mousePressEvent(QMouseEvent* event)
{
    #if _Qt6 /* Qt6 Compat */
    /* Save the last mouse x pos */
    m_LastX = event->position().x();
    #else
    /* Save the last mouse x pos */
    m_LastX = event->x();
    #endif
}

#if _QT6            /* Qt6 Compat*/
void ControlSpinner::mouseMoveEvent(QMouseEvent* event)
{
    /* Get the delta of how much the mouse has moved since the click */
    int delta = event->position().x() - m_LastX;

    /* 
     * If the delta is above the threshold -> we can modify the value 
     * The real threshold can also be altered by pressing in shift
     */   
    // if (std::abs(delta) > (event->modifiers() & Qt::ShiftModifier) ? m_Threshold / 10 : m_Threshold)
    if (std::abs(delta) > m_Threshold)
    {
        /* Check which side the delta has been */
        if (delta > 0)
        {
            /* Set the max of what we get by incrementing or the higher limit */
            setValue(std::min(maximum(), value() + 1));
        }
        else
        {
            /* Set the max of the least or what we get by decrementing */
            setValue(std::max(minimum(), value() - 1));
        }

        /* Update the last mouse x */
        m_LastX = event->position().x();
    }
}
#else
void ControlSpinner::mouseMoveEvent(QMouseEvent* event)
{
    /* Get the delta of how much the mouse has moved since the click */
    int delta = event->x() - m_LastX;

    /* 
     * If the delta is above the threshold -> we can modify the value 
     * The real threshold can also be altered by pressing in shift
     */   
    // if (std::abs(delta) > (event->modifiers() & Qt::ShiftModifier) ? m_Threshold / 10 : m_Threshold)
    if (std::abs(delta) > m_Threshold)
    {
        /* Check which side the delta has been */
        if (delta > 0)
        {
            /* Set the max of what we get by incrementing or the higher limit */
            setValue(std::min(maximum(), value() + 1));
        }
        else
        {
            /* Set the max of the least or what we get by decrementing */
            setValue(std::max(minimum(), value() - 1));
        }

        /* Update the last mouse x */
        m_LastX = event->x();
    }
}
#endif

void ControlSpinner::enterEvent(EnterEvent* event)
{
    /* Set Cursor Override */
    setCursor(Qt::SizeHorCursor);
    QSpinBox::enterEvent(event);
}

void ControlSpinner::leaveEvent(QEvent* event)
{
    setCursor(Qt::ArrowCursor);
    QSpinBox::leaveEvent(event);
}

/* }}} */

VOID_NAMESPACE_CLOSE
