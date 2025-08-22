// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_PUSH_BUTTON_H
#define _VOID_Q_EXT_PUSH_BUTTON_H

/* Qt */
#include <QPushButton>
#include <QToolButton>

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

/**
 * This class just wraps over the standard push button to allow buttons to show a highlight when the button is
 * checked (or toggled visually), this indicates that the button is currently checked
 */
class HighlightToggleButton : public QPushButton
{
public:
    HighlightToggleButton(const std::string& text, QWidget* parent = nullptr);
    HighlightToggleButton(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
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

/**
 * Creates a Close button
 * When hovered will have red background giving an impression of something which
 * should be approached with caution
 */
class CloseButton : public QToolButton
{
public:
    explicit CloseButton(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

/**
 * Creates a Tool Button and a Menu can be attached to it,
 * this only overrides the behaviour which adds arrow to the right side
 * indicating that this button has a menu to not be added
 */
class MenuToolButton : public QToolButton
{
public:
    explicit MenuToolButton(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};

VOID_NAMESPACE_CLOSE

#endif
