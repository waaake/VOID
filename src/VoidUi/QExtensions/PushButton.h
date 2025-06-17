#ifndef _VOID_Q_EXT_PUSH_BUTTON_H
#define _VOID_Q_EXT_PUSH_BUTTON_H

/* Qt */
#include <QPushButton>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Default Color for Toggle */
const QColor TOGGLE_COLOR = QColor(200, 75, 60);

/**
 * This class just wraps over the standard push button to allow buttons to get a different
 * colored text when the toggle state is enabled and keeping the button as-is for the standard state
 */
class ToggleStatePushButton : public QPushButton
{
public:
    ToggleStatePushButton(const std::string& text, QWidget* parent = nullptr, const QColor& color = TOGGLE_COLOR);
    ToggleStatePushButton(QWidget* parent = nullptr, const QColor& color = TOGGLE_COLOR);

    void Toggle(const bool state);

private: /* Members */
    QColor m_Color;
    bool m_ToggleState;

private: /* Methods */
    void Update();

};

VOID_NAMESPACE_CLOSE

#endif
