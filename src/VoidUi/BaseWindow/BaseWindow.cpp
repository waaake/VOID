/* Qt */
#include <QMouseEvent>

/* Internal */
#include "BaseWindow.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* If we're not using framed window -> Going for the custom frameless base */
#ifndef USE_FRAMED_WINDOW

static const int RESIZE_MARGIN = 10;
static const int TITLEBAR_MARGIN = 40;

BaseWindow::BaseWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_Resize(ResizeType::None)
    , m_Dragging(false)
{
    /* Base window needs to be frameless*/
    setWindowFlags(Qt::FramelessWindowHint);
}

#if _QT6        /* Qt 6 Compat */
void BaseWindow::mousePressEvent(QMouseEvent* event)
{
    /* Window geometry */
    QRect window = geometry();

    /* Position for the event click */
    Point pos = event->position();

    /* TODO: Add all types of resize operations */
    if (pos.x() > window.width() - RESIZE_MARGIN && pos.y() > window.height() - RESIZE_MARGIN)   /* Bottom Right */
    {
        m_Resize = ResizeType::BottomRight;
        m_LastPos = event->globalPosition();
    }
    else if (pos.x() < RESIZE_MARGIN && pos.y() > window.height() - RESIZE_MARGIN)               /* Bottom Left */
    {
        m_Resize = ResizeType::BottomLeft;
        m_LastPos = event->globalPosition();
    }
    else if (pos.y() > window.height() - RESIZE_MARGIN)                                          /* Bottom */
    {
        m_Resize = ResizeType::Bottom;
        m_LastPos = event->globalPosition();
    }
    else if (pos.x() > window.width() - RESIZE_MARGIN)                                           /* Right */
    {
        m_Resize = ResizeType::Right;
        m_LastPos = event->globalPosition();
    }
    else if (pos.x() < RESIZE_MARGIN && pos.y() < RESIZE_MARGIN)                                 /* Top Left */
    {
        m_Resize = ResizeType::TopLeft;
        m_LastPos = event->globalPosition();
    }
    else if (pos.x() < RESIZE_MARGIN)                                                            /* Left */
    {
        m_Resize = ResizeType::Left;
        m_LastPos = event->globalPosition();
    }
    else if (event->position().y() < TITLEBAR_MARGIN)                                            /* Title Bar Drag Operation */
    {
        m_Dragging = true;
        m_LastPos = event->globalPosition();
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent* event)
{
    /* Window DRAGGING {{{ */
    if (m_Dragging)
    {
        /* Calculate how much the mouse has moved since last click */
        Point delta = event->globalPosition() - m_LastPos;

        /* And Move the widget/window as well that much */
        move(x() + delta.x(), y() + delta.y());

        /* Reset the last position after the move */
        m_LastPos = event->globalPosition();
    }
    /* }}} */

    /* Window RESIZE {{{ */
    if (m_Resize == ResizeType::BottomRight)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        /* Resize the geometry */
        resize(width() + delta.x(), height() + delta.y());
        /* And update the last pos*/
        m_LastPos = event->globalPosition();
    }
    else if (m_Resize == ResizeType::Bottom)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        /* Resize the geometry -- only in terms of height */
        resize(width(), height() + delta.y());
    
        /* And update the last pos*/
        m_LastPos = event->globalPosition();
    }
    else if (m_Resize == ResizeType::Right)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        /* Resize the geometry -- only in terms of Width */
        resize(width() + delta.x(), height());
    
        /* And update the last pos*/
        m_LastPos = event->globalPosition();
    }
    else if (m_Resize == ResizeType::Left)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        if (width() - delta.x() >= minimumWidth())
        {
           /* 
            * Resize the entire geometry as we're moving the left corner
            * Which means the x pos needs to be changed of the window and also the width
            * in a way that the x + width remains at the same position
            */
            //setUpdatesEnabled(false); // Disable updates temporarily
           resize(width() - delta.x(), height());
           move(x() + delta.x(), y());
           // setGeometry(x() + delta.x(), y(), width() - delta.x(), height());
           /* And update the last pos */
           m_LastPos = event->globalPosition();

           /* Call update to redraw as quickly as we can */
           update();
           
           // setUpdatesEnabled(true);  // Re-enable updates after resizing
        }
    }
    else if (m_Resize == ResizeType::TopLeft)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        if (width() - delta.x() >= minimumWidth() && height() - delta.y() >= minimumHeight())
        {
            /*
             * Resize the entire geometry as we're moving the top-left corner
             * Which means the x pos needs to be changed of the window and also the width
             * and also the y pos needs to be adjuested for the window along with height
             * in a way that the x + width remains at the same position
             */
            resize(width() - delta.x(), height() - delta.y());
            move(x() + delta.x(), y() + delta.y());

            /* And update the last pos */
            m_LastPos = event->globalPosition();

            /* Call update to redraw as quickly as we can */
            update();
        }
    }
    else if (m_Resize == ResizeType::BottomLeft)
    {
        /* Calculate the offset of how much the mouse has moved */
        Point delta = event->globalPosition() - m_LastPos;

        if (width() - delta.x() >= minimumWidth())
        {
            /*
             * Resize the entire geometry as we're moving the bottom-left corner
             * Which means the x pos needs to be changed of the window and also the width
             * and only the height needs adjusting not the ypos
             * in a way that the x + width remains at the same position
             */
            resize(width() - delta.x(), height() + delta.y());
            move(x() + delta.x(), y());

            /* And update the last pos */
            m_LastPos = event->globalPosition();

            /* Call update to redraw as quickly as we can */
            update();
        }
    }
    /* }}} */
}
#else
void BaseWindow::mousePressEvent(QMouseEvent* event)
{
    /* Window geometry */
    QRect window = geometry();

    /* position for the event click */
    Point pos = event->pos();

    /* TODO: Add all types of resize operations */
    if (pos.x() > window.width() - RESIZE_MARGIN && pos.y() > window.height() - RESIZE_MARGIN)    /* Bottom Right */
    {
        m_Resize = ResizeType::BottomRight;
        m_LastPos = event->globalPos();
    }
    else if (pos.x() < RESIZE_MARGIN && pos.y() > window.height() - RESIZE_MARGIN)                /* Bottom Left */
    {
        m_Resize = ResizeType::BottomLeft;
        m_LastPos = event->globalPos();
    }
    else if (pos.y() > window.height() - RESIZE_MARGIN)                                           /* Bottom */
    {
        m_Resize = ResizeType::Bottom;
        m_LastPos = event->globalPos();
    }
    else if (pos.x() > window.width() - RESIZE_MARGIN)                                            /* Right */
    {
        m_Resize = ResizeType::Right;
        m_LastPos = event->globalPos();
    }
    else if (pos.x() < RESIZE_MARGIN && pos.y() < RESIZE_MARGIN)                                  /* Top Left */
    {
        m_Resize = ResizeType::TopLeft;
        m_LastPos = event->globalPos();
    }
    else if (pos.x() < RESIZE_MARGIN)                                                             /* Left */
    {
        m_Resize = ResizeType::Left;
        m_LastPos = event->globalPos();
    }
    else if (event->pos().y() < TITLEBAR_MARGIN)                                                 /* Title Bar Drag Operation */
    {
        m_Dragging = true;
        m_LastPos = event->globalPos();
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent* event)
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
    else if (m_Resize == ResizeType::Right)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        /* Resize the geometry -- only in terms of Width */
        resize(width() + delta.x(), height());
    
        /* And update the last pos*/
        m_LastPos = event->globalPos();
    }
    else if (m_Resize == ResizeType::Left)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        if (width() - delta.x() >= minimumWidth())
        {
           /* 
            * Resize the entire geometry as we're moving the left corner
            * Which means the x pos needs to be changed of the window and also the width
            * in a way that the x + width remains at the same position
            */
            //setUpdatesEnabled(false); // Disable updates temporarily
           resize(width() - delta.x(), height());
           move(x() + delta.x(), y());
           // setGeometry(x() + delta.x(), y(), width() - delta.x(), height());
           /* And update the last pos */
           m_LastPos = event->globalPos();

           /* Call update to redraw as quickly as we can */
           update();
           
           // setUpdatesEnabled(true);  // Re-enable updates after resizing
        }
    }
    else if (m_Resize == ResizeType::TopLeft)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        if (width() - delta.x() >= minimumWidth() && height() - delta.y() >= minimumHeight())
        {
            /*
             * Resize the entire geometry as we're moving the top-left corner
             * Which means the x pos needs to be changed of the window and also the width
             * and also the y pos needs to be adjuested for the window along with height
             * in a way that the x + width remains at the same position
             */
            resize(width() - delta.x(), height() - delta.y());
            move(x() + delta.x(), y() + delta.y());

            /* And update the last pos */
            m_LastPos = event->globalPos();

            /* Call update to redraw as quickly as we can */
            update();
        }
    }
    else if (m_Resize == ResizeType::BottomLeft)
    {
        /* Calculate the offset of how much the mouse has moved */
        QPoint delta = event->globalPos() - m_LastPos;

        if (width() - delta.x() >= minimumWidth())
        {
            /*
             * Resize the entire geometry as we're moving the bottom-left corner
             * Which means the x pos needs to be changed of the window and also the width
             * and only the height needs adjusting not the ypos
             * in a way that the x + width remains at the same position
             */
            resize(width() - delta.x(), height() + delta.y());
            move(x() + delta.x(), y());

            /* And update the last pos */
            m_LastPos = event->globalPos();

            /* Call update to redraw as quickly as we can */
            update();
        }
    }
    /* }}} */
}
#endif  // _QT6

void BaseWindow::mouseReleaseEvent(QMouseEvent* event)
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

#endif // USE_FRAMED_WINDOW

VOID_NAMESPACE_CLOSE
