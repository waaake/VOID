/* Internal */
#include "PushButton.h"

VOID_NAMESPACE_OPEN

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

VOID_NAMESPACE_CLOSE
