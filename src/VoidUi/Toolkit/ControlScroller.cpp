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
    /* Base Mouse Press */
    QSpinBox::mousePressEvent(event);

    #if _QT6 /* Qt6 Compat */
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
            setValue(std::min(maximum(), value() + singleStep()));
        }
        else
        {
            /* Set the max of the least or what we get by decrementing */
            setValue(std::max(minimum(), value() - singleStep()));
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
            setValue(std::min(maximum(), value() + singleStep()));
        }
        else
        {
            /* Set the max of the least or what we get by decrementing */
            setValue(std::max(minimum(), value() - singleStep()));
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

/* Quick Spinner {{{ */

QuickSpinner::QuickSpinner(QWidget* parent)
    : QuickSpinner("", 0.0, 10.0, 0.0, parent)
{
}

QuickSpinner::QuickSpinner(const std::string& label, const double min, const double max, const double value, QWidget* parent)
    : QWidget(parent)
    , m_Default(value)
    , m_Last(value)
{
    /* Build the UI elements */
    Build();

    /* Setup Values */
    m_Spinner->setMinimum(min);
    m_Spinner->setMaximum(max);

    /* The value acts as the default value and last as well */
    m_Spinner->setValue(value);

    /* Connect */
    connect(m_Spinner, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &QuickSpinner::ValueChanged);
    connect(m_Label, &ClickableLabel::clicked, this, &QuickSpinner::Reset);
}

QuickSpinner::~QuickSpinner()
{
    m_Label->deleteLater();
    delete m_Label;
    m_Label = nullptr;

    m_Spinner->deleteLater();
    delete m_Spinner;
    m_Spinner = nullptr;

    m_Layout->deleteLater();
    delete m_Layout;
    m_Layout = nullptr;
}

void QuickSpinner::Build()
{
    /* Main Layout */
    m_Layout = new QHBoxLayout(this);

    /* Spacing */
    m_Layout->setContentsMargins(0, 0, 0, 0);
    m_Layout->setSpacing(1);

    m_Label = new ClickableLabel;

    m_Spinner = new ControlDoubleSpinner;
    
    /* Add to the main layout */
    m_Layout->addWidget(m_Label);
    m_Layout->addWidget(m_Spinner);
}

void QuickSpinner::ValueChanged(const double value)
{
    QPalette p = m_Label->palette();

    /* Validate the value against the last value to set the Color of the Spinner */
    if (m_Default != value)
    {
        p.setColor(QPalette::WindowText, Qt::red);

        /* Update the Last value -- The last value should never be the default value */
        m_Last = value;
    }
    else
    {
        p.setColor(QPalette::WindowText, QColor(210, 210, 210));
    }
    
    /* Update the color on the Label to indicate when we're on the Default value */
    m_Label->setPalette(p);

    /* Emit that the value has been changed */
    emit valueChanged(value);
}

void QuickSpinner::Reset()
{
    /* If we're already in the default value -> switch to the last value */
    if (m_Spinner->value() == m_Default)
        m_Spinner->setValue(m_Last);
    else
        m_Spinner->setValue(m_Default);
}

/* }}} */

VOID_NAMESPACE_CLOSE
