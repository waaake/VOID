// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_FRAMELESS_WINDOW_H
#define _VOID_FRAMELESS_WINDOW_H

/* Qt */
#include <QMainWindow>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

#ifdef USE_FRAMED_WINDOW
class VOID_API BaseWindow : public QMainWindow
{
public:
    BaseWindow(QWidget* parent = nullptr) : QMainWindow(parent) {}
};
#else
class VOID_API BaseWindow : public QMainWindow
{
public:
    BaseWindow(QWidget* parent = nullptr);

protected:
    /* 
     * Overriding the mouse movements and triggers 
     * to allow for the base widget functionality like resizing
     * or dragging from the top title bar
     */
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private: /* enum */
    /* Different Type of possible resizing operations */
    enum class ResizeType: short
    {
        /* Nothing is being resized */
        None,

        /* Possible types of resize operations */
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left,
    };

private: /* members */
    ResizeType m_Resize;
    Point m_LastPos;

    bool m_Dragging;

};
#endif // USE_FRAMED_WINDOW

VOID_NAMESPACE_CLOSE

#endif // _VOID_FRAMELESS_WINDOW_H
