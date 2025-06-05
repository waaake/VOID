/* Qt */
#include <QMouseEvent>

/* Internal */
#include "FramelessWindow.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

static const int RESIZE_MARGIN = 10;
static const int TITLEBAR_MARGIN = 40;

FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_Resize(ResizeType::None)
    , m_Dragging(false)
{
    /* Base window needs to be frameless*/
    setWindowFlags(Qt::FramelessWindowHint);
}

void FramelessWindow::mousePressEvent(QMouseEvent* event)
{
    /* Window geometry */
    QRect window = geometry();

    /* TODO: Add all types of resize operations */
    if (event->x() > window.width() - RESIZE_MARGIN && event->y() > window.height() - RESIZE_MARGIN)
    {
        m_Resize = ResizeType::BottomRight;
        m_LastPos = event->globalPos();
    }
    else if (event->y() > window.height() - RESIZE_MARGIN)
    {
        m_Resize = ResizeType::Bottom;
        m_LastPos = event->globalPos();
    }
    else if (event->y() < TITLEBAR_MARGIN)
    {
        m_Dragging = true;
        m_LastPos = event->globalPos();
    }
    else if (event->x() > window.width() - RESIZE_MARGIN)
    {
        m_Resize = ResizeType::Left;
        m_LastPos = event->globalPos();
    }
}

void FramelessWindow::mouseMoveEvent(QMouseEvent* event)
{
    /* Window DRAGGING {{{ */
    if (m_Dragging)
    {
        /* Calculate how much the mouse has moved since last click */
        QPoint delta = event->globalPos() - m_LastPos;

        /* And Move the widget/window as well that much */
        move(x() + delta.x(), y() + delta.y());

        /* Reset the last position after the move */
        m_LastPos = event->globalPos();
    }
    /* }}} */

    /* Window RESIZE {{{ */
    if (m_Resize == ResizeType::BottomRight)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        /* Resize the geometry */
        resize(width() + delta.x(), height() + delta.y());
        /* And update the last pos*/
        m_LastPos = event->globalPos();
    }
    else if (m_Resize == ResizeType::Bottom)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        /* Resize the geometry -- only in terms of height */
        resize(width(), height() + delta.y());
    
        /* And update the last pos*/
        m_LastPos = event->globalPos();
    }
    else if (m_Resize == ResizeType::Left)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        /* Resize the geometry -- only in terms of Width */
        resize(width() + delta.x(), height());
    
        /* And update the last pos*/
        m_LastPos = event->globalPos();
    }
    /* }}} */
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_Resize != ResizeType::None)
    {
        /* Restore the Resize Type */
        m_Resize = ResizeType::None;
    }

    if (m_Dragging)
    {
        /* Reset Dragging */
        m_Dragging = false;
    }

}

VOID_NAMESPACE_CLOSE
