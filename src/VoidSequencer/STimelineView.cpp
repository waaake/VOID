// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>

/* Internal */
#include "SContext.h"
#include "STimelineView.h"
#include "STimelineScene.h"
#include "Internal/Descriptors.h"
#include "Graphics/STrackItem.h"
#include "VoidCore/Logging.h"
#include "VoidIconForge/IconForge.h"
#include "VoidMediaPlayer/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

STimelineView::STimelineView(SequencerContext* context, QWidget* parent)
    : QGraphicsView(parent)
    , m_Context(context)
{
    Build();
    Setup();
}

void STimelineView::AddPlayhead()
{
    m_Scene->AddPlayhead();
}

void STimelineView::AddTrack(const SharedPlaybackTrack& track)
{
    m_Scene->AddTrack(track);
}

void STimelineView::RemoveTrack(const SharedPlaybackTrack& track)
{
    m_Scene->RemoveTrack(track);
}

void STimelineView::Refresh()
{
    m_Scene->UpdateItems();
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

MFrameRange STimelineView::VisibleRange() const
{
    QRect viewrect = viewport()->rect();

    return MFrameRange(
        m_Context->Geometry()->SceneXToFrame(mapToScene(viewrect.topLeft()).x()),
        m_Context->Geometry()->SceneXToFrame(mapToScene(viewrect.topRight()).x() + style()->pixelMetric(QStyle::PM_ScrollBarExtent))
    );
}

void STimelineView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(MimeTypes::MediaItem))
    {
        event->acceptProposedAction();
        QByteArray data = event->mimeData()->data(MimeTypes::MediaItem);
        m_Scene->InitDraggableItems(_MediaBridge.UnpackProjectMedia(data));
    }
}

void STimelineView::dragMoveEvent(QDragMoveEvent* event)
{
    m_Scene->MoveDraggableItems(mapToScene(event->pos()));
}

void STimelineView::dragLeaveEvent(QDragLeaveEvent* event)
{
    m_Scene->DestroyDraggableItems();
}

void STimelineView::dropEvent(QDropEvent* event)
{
    m_Scene->DropItems(mapToScene(event->pos()));
}

void STimelineView::mousePressEvent(QMouseEvent* event)
{
    if (m_Context->Action() == SequencerAction::RAZOR_ALL)
        emit sequenceCutRequested(m_Context->Geometry()->SceneXToFrame(mapToScene(event->pos()).x()));

    if (event->button() == Qt::LeftButton && m_Context->Action() == SequencerAction::NONE && !m_Scene->itemAt(mapToScene(event->pos()), QTransform()))
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
        m_Scene->SelectItems(marquee.boundingRect());

        viewport()->update();
    }

    if (m_Context->Action() == SequencerAction::RAZOR_ALL)
        m_Scene->SetRazorX(mapToScene(event->pos()).x());

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

void STimelineView::enterEvent(EnterEvent* event)
{
    QGraphicsView::enterEvent(event);

    switch (m_Context->Action())
    {
        case SequencerAction::SLIP_CLIP:
            setCursor(QCursor(IconForge::GetPixmap(IconType::icon_arrow_range, _DARK_COLOR(QPalette::Text, 100))));
            break;
        case SequencerAction::TRIM:
            setCursor(QCursor(IconForge::GetPixmap(IconType::icon_format_overflow, _DARK_COLOR(QPalette::Text, 100))));
            break;
        case SequencerAction::RAZOR:
            setCursor(QCursor(IconForge::GetPixmap(IconType::icon_bolt, _DARK_COLOR(QPalette::Text, 100))));
            break;
        case SequencerAction::RAZOR_ALL:
            setCursor(QCursor(IconForge::GetPixmap(IconType::icon_bolt, _DARK_COLOR(QPalette::Text, 100))));
            m_Scene->ToggleRazorhead(true);
            break;
        case SequencerAction::MERGE:
            setCursor(QCursor(IconForge::GetPixmap(IconType::icon_cell_merge, _DARK_COLOR(QPalette::Text, 100))));
            break;
    }
}

void STimelineView::leaveEvent(QEvent* event)
{
    QGraphicsView::leaveEvent(event);
    unsetCursor();

    if (m_Context->Action() == SequencerAction::RAZOR_ALL)
        m_Scene->ToggleRazorhead(false);
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

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setAcceptDrops(true);
}

VOID_NAMESPACE_CLOSE
