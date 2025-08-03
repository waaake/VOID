// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_PUSH_BUTTON_H
#define _VOID_Q_EXT_PUSH_BUTTON_H

/* Qt */
#include <QPushButton>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Default Color for Toggle */
const QColor TOGGLE_COLOR = QColor(200, 75, 60);
const QColor HIGHLIGHT_COLOR = QColor(160, 190, 60);

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

/**
 * This class just wraps over the standard push button to allow buttons to show a highlight when the button is
 * checked (or toggled visually), this indicates that the button is currently checked
 */
class HighlightToggleButton : public QPushButton
{
public:
    HighlightToggleButton(const std::string& text, QWidget* parent = nullptr, const QColor& color = HIGHLIGHT_COLOR);
    HighlightToggleButton(QWidget* parent = nullptr, const QColor& color = HIGHLIGHT_COLOR);

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    QColor m_Color;
};

/**
 * This class creates a Tool button which when clicked shows a ColorDialog allowing selection of color
 * The selected color is what is displayed on the button as well
 */
class ColorSelectionButton : public QPushButton
{
    Q_OBJECT

public:
    ColorSelectionButton(QWidget* parent = nullptr);
    ColorSelectionButton(const QColor& color, QWidget* parent = nullptr);

signals:
    /**
     * Emits the currently selected color
     */
    void colorChanged(const QColor&);

protected:
    void paintEvent(QPaintEvent* event) override;

private: /* Members */
    QColor m_Color;

private: /* Methods */
    /**
     * Gets invoked to allow selection of color and emits the selected color.
     */
    void SelectColor();
};

VOID_NAMESPACE_CLOSE

#endif
