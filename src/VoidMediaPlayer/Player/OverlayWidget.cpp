// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "OverlayWidget.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

PlayerOverlay::PlayerOverlay(QWidget* parent)
    : QWidget(parent)
    , m_MediaDragHighlight(false)
    , m_HoveredViewerBuffer(HoveredViewerBuffer::None)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
}

void PlayerOverlay::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    if (m_HoveredViewerBuffer == HoveredViewerBuffer::None)
        return;

    QColor colorA = (m_HoveredViewerBuffer == HoveredViewerBuffer::A) ? QColor(130, 110, 190, 150) : QColor(130, 110, 190, 80);
    QColor colorB = (m_HoveredViewerBuffer == HoveredViewerBuffer::B) ? QColor(70, 180, 220, 150) : QColor(70, 180, 220, 80);

    painter.fillRect(m_RectA, colorA);
    painter.fillRect(m_RectB, colorB);

    QFont f = painter.font();
    f.setPixelSize(60);
    f.setBold(true);

    painter.setFont(f);

    painter.setPen(QColor(255, 255, 255, 130));
    painter.drawText(m_RectA, Qt::AlignCenter, "A");
    painter.drawText(m_RectB, Qt::AlignCenter, "B");
}

void PlayerOverlay::ResetRect()
{
    QRect r = rect();
    m_RectA = QRect(r.left(), r.top(), r.width() * 0.5, r.height());
    m_RectB = QRect(m_RectA.right(), r.top(), r.width() * 0.5, r.height());
}

void PlayerOverlay::resizeEvent(QResizeEvent* event)
{
    ResetRect();
    QWidget::resizeEvent(event);
}

void PlayerOverlay::SetHoveredBuffer(const QPoint& pos)
{
    if (m_RectA.contains(pos))
        m_HoveredViewerBuffer = HoveredViewerBuffer::A;
    else if (m_RectB.contains(pos))
        m_HoveredViewerBuffer = HoveredViewerBuffer::B;
    else
        m_HoveredViewerBuffer = HoveredViewerBuffer::None;

    /* Repaint */
    update();
}

void PlayerOverlay::SetHoveredBuffer(const HoveredViewerBuffer& buffer)
{
    m_HoveredViewerBuffer = buffer;

    /* Repaint */
    update();
}

VOID_NAMESPACE_CLOSE
