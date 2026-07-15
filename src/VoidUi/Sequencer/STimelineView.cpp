// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>
#include <QMouseEvent>

/* Internal */
#include "SContext.h"
#include "STimelineView.h"
#include "STimelineScene.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

STimelineView::STimelineView(SequencerContext* context, QWidget* parent)
    : QGraphicsView(parent)
    , m_Context(context)
{
    Build();
    Setup();
}

void STimelineView::SetSequence(const SharedPlaybackSequence& sequence)
{
    m_Scene->SetSequence(sequence);
    m_Scene->AddPlayhead();
}

void STimelineView::Clear()
{
    m_Scene->Clear();
}

void STimelineView::Focus()
{
    const auto& selected = m_Scene->selectedItems();
    selected.empty() ? centerOn(0, 0) : centerOn(selected[0]);
}

void STimelineView::mousePressEvent(QMouseEvent* event)
{
    if (!m_Scene->itemAt(event->pos(), QTransform()))
    {
        m_Marquee.pressed = true;
        m_Marquee.clickpos = event->pos();
    }

    QGraphicsView::mousePressEvent(event);
}

void STimelineView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_Marquee.pressed)
    {
        QPoint delta = m_Marquee.clickpos - event->pos();
        if (delta.manhattanLength() > 10)
        {
            m_Marquee.pressed = false;
            m_Marquee.active = true;
        }
    }

    if (m_Marquee.active)
    {
        m_Marquee.rect = QRect(m_Marquee.clickpos, event->pos()).normalized();
        QPolygonF marquee = mapToScene(m_Marquee.rect);
        m_Scene->SelectTrackItems(marquee.boundingRect());

        viewport()->update();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void STimelineView::mouseReleaseEvent(QMouseEvent* event)
{
    m_Marquee.pressed = false;
    if (m_Marquee.active)
    {
        m_Marquee.active = false;
        m_Marquee.rect = QRect();

        viewport()->update();
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void STimelineView::drawForeground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawForeground(painter, rect);

    if (m_Marquee.active)
    {
        QColor color = palette().color(QPalette::Highlight);
        painter->setPen(color);

        color.setAlpha(40);
        painter->setBrush(color);

        painter->drawRect(mapToScene(m_Marquee.rect).boundingRect());
    }
}

void STimelineView::Build()
{
    m_Scene = new STimelineScene(m_Context, this);
    setScene(m_Scene);

    m_Scene->AddPlayhead();
}

void STimelineView::Setup()
{
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);

    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::NoDrag);
    setFrameShape(QFrame::NoFrame);

    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setCacheMode(QGraphicsView::CacheBackground);
}

VOID_NAMESPACE_CLOSE
