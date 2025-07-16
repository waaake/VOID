/* Qt */
#include <QPainter>
#include <QPaintEvent>
#include <QColorDialog>

/* Internal */
#include "PushButton.h"

VOID_NAMESPACE_OPEN

/* Toggle State Push Button {{{ */
ToggleStatePushButton::ToggleStatePushButton(const std::string& text, QWidget* parent, const QColor& color)
    : QPushButton(text.c_str(), parent)
    , m_Color(color)
    , m_ToggleState(false)
{
}

ToggleStatePushButton::ToggleStatePushButton(QWidget* parent, const QColor& color)
    : ToggleStatePushButton("", parent, color)
{
}


void ToggleStatePushButton::Toggle(const bool state)
{
    /* Update the toggle state */
    m_ToggleState = state;

    /* Update the color */
    Update();
}

void ToggleStatePushButton::Update()
{
    /* Based on the toggle state -> We change the color of the text on the button */
    if (m_ToggleState)
    {
        /* Get the current palette */
        QPalette p(palette());
        p.setColor(QPalette::ButtonText, m_Color);

        /* Apply the modified color */
        setPalette(p);
    }
    else
    {
        /* Reset to the default palette */
        setPalette(QPalette());
    }
}

/* }}} */

/* Toggle State Push Button {{{ */
HighlightToggleButton::HighlightToggleButton(const std::string& text, QWidget* parent, const QColor& color)
    : QPushButton(text.c_str(), parent)
    , m_Color(color)
{
    /* This needs to be checkable */
    setCheckable(true);
    /* And Flat */
    setFlat(true);
}

HighlightToggleButton::HighlightToggleButton(QWidget* parent, const QColor& color)
    : HighlightToggleButton("", parent, color)
{
}


void HighlightToggleButton::paintEvent(QPaintEvent* event)
{
    /* Base Painting */
    QPushButton::paintEvent(event);

    /* Custom Paint to add a highlight line on the bottom */
    if (isChecked())
    {
        QPainter painter(this);

        /* A rect which is just super thin in terms of height at the bottom */
        QRectF r(rect());
        r.setTop(r.bottom() - 2);

        painter.fillRect(r, m_Color);
    }
}

/* }}} */

/* Color Selection Button {{{ */

ColorSelectionButton::ColorSelectionButton(QWidget* parent)
    : ColorSelectionButton({0, 0, 0}, parent)
{
}

ColorSelectionButton::ColorSelectionButton(const QColor& color, QWidget* parent)
    : QPushButton(parent)
    , m_Color(color)
{
    /* Connect to allow selecting Color */
    connect(this, &QPushButton::clicked, this, &ColorSelectionButton::SelectColor);
}

void ColorSelectionButton::paintEvent(QPaintEvent* event)
{
    /* Add the current color as what'll be displayed on the button */
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.setBrush(QBrush(m_Color));

    painter.drawRect(rect());
}

void ColorSelectionButton::SelectColor()
{
    QColor selected = QColorDialog::getColor(m_Color, this, "Select Color");

    /* If we have a valid and different color selected */
    if (selected.isValid() && selected != m_Color)
    {
        /* Update the color */
        m_Color = selected;
        /* Emit the updated color */
        emit colorChanged(m_Color);
        /* And repaint */
        update();
    }
}

/* }}} */

VOID_NAMESPACE_CLOSE
